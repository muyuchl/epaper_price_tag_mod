#include <MSP430G2553.h>
#include "a7105.h"

const unsigned char id_table[4] = {0x11, 0x22, 0x33, 0x44};

const uint8_t RF_CHANNEL = 20;



static void delay_us( unsigned int k )
{
    for (int j = 0; j < k; j++)
    {
      _NOP();
    }
}

static void WaitBit_0(uint8_t reg, uint8_t nbit)
{
	while (a7105_read_reg(reg) & nbit)
		;
}

static void SetIVCOBand(uint8_t vb1, uint8_t vb2);
static const uint8_t   A7105Config[51];

void a7105_init()
{

    // spi ncs
    SCS_H;

    SCS_UP;
    SCS_DirOut;
    

     // SCLK
    SCLK_H;

     SCLK_DirOut;
     SCLK_UP;
     


     // MOSI
     SDIO_H;

     SDIO_UP;
     SDIO_DirOut;
     

     // GIO1 MISO
     GIO1_DirIn;
     GIO1_PULLUP;

     // GIO2    WTR
     GIO2_DirIn;
     GIO2_PULLUP;
     
     delay_us(1);

	a7105_reset();

	delay_us(100);     

	a7105_write_id(id_table);
     
	 a7105_config();
 	a7105_cal();	

	delay_us(1); 
}

void a7105_reset()
{
	a7105_write_reg(MODE_REG, 0x00);
}

void a7105_write_id(const unsigned char *id_data)
{
	SCS_L;

	a7105_send_byte(IDCODE_REG);

	for (int i = 0; i < 4; i++) {
		a7105_send_byte(id_data[i]);
	}

	SCS_H;
}

void a7105_read_id(unsigned char *id_data)
{
	SCS_L;

	a7105_send_byte(0x40 | IDCODE_REG);	// read register

	for (int i = 0; i < 4; i++) {
		id_data[i] = a7105_read_byte();
	}

	SCS_H;
}

void a7105_send_byte(unsigned char data)
{
	for (int i = 0; i < 8; i++)
	{
		if (data & 0x80) {
			SDIO_H;
		} else {
			SDIO_L;
		}

		delay_us(2);
		SCLK_H;
		delay_us(2);
		SCLK_L;

		data = data << 1;
	}
}


unsigned char a7105_read_byte()
{
	unsigned char data = 0;

	delay_us(2);

	for (int i = 0; i < 8; i++) {
		if (GIO1_Data) {
			data = (data << 1) | 0x01;
		} else {
			data = data << 1;
		}


		SCLK_H;
		delay_us(2);
		SCLK_L;
		delay_us(2);
	}

	return data;
}

void a7105_write_reg(const unsigned char reg, unsigned char value)
{

SCS_L;

a7105_send_byte(reg);
a7105_send_byte(value);

SCS_H;
}

unsigned char a7105_read_reg(const unsigned char reg)
{
	unsigned char ret = 0;

SCS_L;

a7105_send_byte(reg | 0x40);
ret = a7105_read_byte();

SCS_H;

return ret;
}


void a7105_config()
{
uint8_t i = 0;

	// 0x00 mode register, for reset
	// 0x05 fifo data register
	// 0x06 id code register
	// 0x23 IF calibration II, only read
	// 0x32 filter test register

	for (i = 0x01; i <= 0x04; i++)
	{
		a7105_write_reg(i, A7105Config[i]);
	}

	for (i = 0x07; i <= 0x22; i++)
	{
		a7105_write_reg(i, A7105Config[i]);
	}

	for (i = 0x24; i <= 0x31; i++)
	{
		a7105_write_reg(i, A7105Config[i]);
	}
}

void a7105_cal()
{
	uint8_t tmp = 0;
	uint8_t fb, fbcf;
	uint8_t vb1, vbcf1, dvt1;
	uint8_t vb2, vbcf2, dvt2;

	a7105_strobe_cmd(CMD_STBY);	// calibration @ STB state

	// calibration IF Procedure
	a7105_write_reg(CALIBRATION_REG, 0x01);
	WaitBit_0(CALIBRATION_REG, 0x01);

	// for check
	tmp = a7105_read_reg(IFCAL1_REG);
	fb = tmp & 0x0F;
	fbcf = (tmp >> 4) & 0x01;

	if (fbcf == 1)
	{
		// debuglog("calib IF fail \r\n");
		return;
	}

	// calibration vco procedure
	a7105_write_reg(VCOCCAL_REG, 0x13);	// manual VCOC = 3
	a7105_write_reg(VCOCAL2_REG, 0x3B);	// VTL = 3, VTH = 7

	a7105_set_ch(0);		// setting 2400MHz
	a7105_write_reg(CALIBRATION_REG, 0x02);
	WaitBit_0(CALIBRATION_REG, 0x02);

	tmp = a7105_read_reg(VCOCAL1_REG);
	vb1 = tmp & 0x07;
	vbcf1 = (tmp >> 3) & 0x01;
	dvt1 = (tmp >> 4) & 0x03;

	a7105_set_ch(160);	// setting 2480MHz
	a7105_write_reg(CALIBRATION_REG, 0x02);
	WaitBit_0(CALIBRATION_REG, 0x02);

	tmp = a7105_read_reg(VCOCAL1_REG);
	vb2 = tmp & 0x07;
	vbcf2 = (tmp >> 3) & 0x01;
	dvt2 = (tmp >> 4) & 0x03;

	SetIVCOBand(vb1, vb2);

	if (vbcf1 == 1 && vbcf2 == 1)
	{
		// debuglog("calib vbcf1 vbcf2 == 1 \r\n");
		return;
	}
}

// void a7105_RCO_cal()
// {

// }

void a7105_set_ch(uint8_t ch)
{
a7105_write_reg(PLL1_REG, ch);
}

void a7105_toRxMode()
{
	a7105_set_ch(RF_CHANNEL - 1);
    a7105_strobe_cmd(CMD_RX);
}

void a7105_strobe_cmd(uint8_t cmd)
{
SCS_L;
a7105_send_byte(cmd);
	
	SCS_H;
}

uint8_t a7105_gio2_low()
{
	if (A7105_GIO2_IS_LOW) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t a7105_rx_crc_ok()
{
uint8_t value = a7105_read_reg(MODE_REG) & (0x20);

	if (value == 0) {
		// crc ok
		return 1;
	} else {
		return 0;
	}
}

void a7105_read_rxdata(uint8_t *data, int len)
{
	uint8_t cmd = 0x40 | FIFO_REG;

	SCS_L;

	a7105_send_byte(cmd);
	

	if (len > 64)
	{
		len = 64;
	}

	for (int i = 0; i < len; i++) {
		data[i] = a7105_read_byte();
	}

	

	SCS_H;
}

void a7105_tx_data(uint8_t *data, int len)
{
a7105_strobe_cmd(CMD_STBY);

	    a7105_writefifo(data, len);

	    a7105_set_ch(RF_CHANNEL);

	    a7105_strobe_cmd(CMD_TX);

	    
	    while (A7105_BUSY)
	    {
	        ;
	    }
}

// internal
void a7105_writefifo(uint8_t *data, int len)
{
	uint8_t cmd = FIFO_REG;

	if (len > 64) {
		len = 64;
	}

	SCS_L;

	a7105_send_byte(cmd);

	for (int i = 0; i < len; i++) {
		a7105_send_byte(data[i]);
	}
	

	SCS_H;
}


static void SetIVCOBand(uint8_t vb1, uint8_t vb2)
{
	uint8_t diff1, diff2;

	if (vb1 >= 4) {
		diff1 = vb1 - 4;
	} else {
		diff1 = 4 - vb1;
	}

	if (vb2 >= 4) {
		diff2 = vb2 - 4;
	} else {
		diff2 = 4 - vb2;
	}

	if (diff1 == diff2  || diff1 > diff2)
	{
		a7105_write_reg(VCOCAL1_REG, (vb1 | 0x08));
	}
	else
	{
		a7105_write_reg(VCOCAL1_REG, (vb2 | 0x08));
	}
}

static const uint8_t   A7105Config[51] =
{
		0x00, //RESET register,         only reset, not use on config
		0x42, //MODE reaister
		0x00, //CALIBRATION register,only read, not use on config
		0x3F, //FIFO1 register,
		0x00, //FIFO2 register,
		0x00, //FIFO register, for fifo read/write
		0x00, //IDDATA register, for idcode
		0x00, //RCOSC1 register,
		0x00, //RCOSC2 register,
		0x00, //RCOSC3 register,
		0x00, //CKO register,
		0x19,  // 0x01, //GPIO1 register		// slave out
	   0x01,   	//0x21, //GPIO2 register,
		0x05, //CLOCK register,
		0x00, //DATARATE register,
		0x50, //PLL1 register,
		0x9E, //PLL2 register   RFbase 2400MHz
		0x4B, //PLL3 register
		0x00, //PL4 register
		0x02, //PLL5 register,
		0x16, //TX1 register,
		0x2B, //TX2 register,
		0x12, //DELAY1 register,
		0x00, //IDELAY2 register,
		0x62, //RX register,
		0x80, //RXGAIN1 register
		0x80, //RXGAIN2 register,
		0x00, //RXGAIN3 register,
		0x0A, //RXGAIN4 register
		0x32, //RSSI register,
		0xC3, //ADC reaister,
		0x0F, //I   CODE1 register,  enable CRC
		0x16, //CODE2 register,
		0x00, //CODE3 register,
		0x00, //IFCAL1 register, only read
		0x00, //FCAL2 register, only read
		0x00, //VCOCCAL register
		0x00, //IVCOCAL1 register,
		0x3B, //IVCOCAL2 register,
		0x00, //BATTERY register,
		0x17, //TXTEST register,
		0x47, //RXDEM1 register
		0x80, //RXDEM2 register,
		0x03, //CPC register,
		0x01, //CRYSTAL register
		0x45, //PLLTEST reaister,
		0x18, //IVCOTEST1 register,
		0x00, //IVCOTEST2 register,
		0x01, //IFAT register,
		0x0F, //IRSCALE register,
		0x00, //FILTERTEST

};
