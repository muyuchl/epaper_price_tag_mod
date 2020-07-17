/*
 * a7105.c
 *
 *  Created on: Jul 1, 2020
 *      Author: chl
 */


#include "a7105.h"
#include "a7105reg.h"
#include "main.h"
#include "logdebug.h"

#include <stdio.h>


extern SPI_HandleTypeDef hspi1;

const uint8_t RF_CHANNEL = 20;

static const uint8_t   A7105Config[51];

uint8_t id_table[4] = {0x11, 0x22, 0x33, 0x44};

#define RF_CS_LOW HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define RF_CS_HI HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)


static void WaitBit_0(uint8_t reg, uint8_t nbit)
{
	while (A7105_ReadReg(reg) & nbit)
		;
}

static void SetIVCOBand(uint8_t vb1, uint8_t vb2);


void initRF(void)
{
	// pin should already configured by CubeMX

	A7105_Reset();
	HAL_Delay(100);

	A7105_WriteID();

	A7105_Config();
	A7105_Cal();
	HAL_Delay(100);

	A7105_ReadID();
}

void A7105_Reset(void)
{
	// reset RF chip
	A7105_WriteReg(MODE_REG, 0x00);
}

void A7105_ToRxMode(void)
{
	SetCH(RF_CHANNEL - 1);
    StrobeCmd(CMD_RX);

}

void A7105_TxData(uint8_t * data, int len)
{

	StrobeCmd(CMD_STBY);

	    A7105_WriteFIFO(data, len);

	    SetCH(RF_CHANNEL);

	    StrobeCmd(CMD_TX);
	    while (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
	    {
	        ;
	    }

}

void A7105_WriteReg(uint8_t address, uint8_t value)
{
	uint8_t buf[4];

	buf[0] = address;
	buf[1] = value;

	RF_CS_LOW;

	HAL_SPI_Transmit(&hspi1, buf, 2, 2 * 1000);

	RF_CS_HI;
}

uint8_t A7105_ReadReg(uint8_t addr)
{

	uint8_t buf[2] = {0};
	uint8_t rxbuf[2] = {0};

	RF_CS_LOW;

	buf[0] = 0x40 | addr;       // read register
	HAL_SPI_TransmitReceive(&hspi1, buf, rxbuf, 2, 2 * 1000);

	RF_CS_HI;

	return rxbuf[1];
}

void ByteSend(uint8_t src)
{
	HAL_SPI_Transmit(&hspi1, &src, 1, 2 * 1000);
}

uint8_t ByteRead(void)
{
	uint8_t ret = 0;
	HAL_SPI_Receive(&hspi1, &ret, 1, 2 * 1000);
	return ret;
}

void A7105_WriteID(void)
{
	uint8_t buf[5];

	buf[0] = IDCODE_REG;	// reg addr
	buf[1] = id_table[0];	// id data byte 0
	buf[2] = id_table[1];
	buf[3] = id_table[2];
	buf[4] = id_table[3];	// id data byte 3

	RF_CS_LOW;

	HAL_SPI_Transmit(&hspi1, buf, 5, 2 * 1000);

	RF_CS_HI;
}

void A7105_ReadID(void)
{
	uint8_t buf[5];

	buf[0] = 0x40 | IDCODE_REG;	// reg addr
	//		buf[1] = id_table[0];	// id data byte 0
	//		buf[2] = id_table[1];
	//		buf[3] = id_table[2];
	//		buf[4] = id_table[3];	// id data byte 3

	RF_CS_LOW;

	// send addr
	HAL_SPI_Transmit(&hspi1, buf, 1, 2 * 1000);

	// read data
	HAL_SPI_Receive(&hspi1, buf, 4, 2 * 1000);

	char cbuf[60];
	int slen = sprintf(cbuf, "read id:  %02X %02X %02X %02X \r\n", buf[0], buf[1], buf[2], buf[3]);
	debuglog(cbuf);

	RF_CS_HI;
}

void A7105_WriteFIFO(uint8_t *data, int len)
{
	uint8_t cmd = FIFO_REG;

	if (len > 64) {
		len = 64;
	}

	RF_CS_LOW;

	HAL_SPI_Transmit(&hspi1, &cmd, 1, 2 * 1000);
	HAL_SPI_Transmit(&hspi1, data, len, 2 * 1000);

	RF_CS_HI;
}


void A7105_Config(void)
{
	uint8_t i = 0;

	// 0x00 mode register, for reset
	// 0x05 fifo data register
	// 0x06 id code register
	// 0x23 IF calibration II, only read
	// 0x32 filter test register

	for (i = 0x01; i <= 0x04; i++)
	{
		A7105_WriteReg(i, A7105Config[i]);
	}

	for (i = 0x07; i <= 0x22; i++)
	{
		A7105_WriteReg(i, A7105Config[i]);
	}

	for (i = 0x24; i <= 0x31; i++)
	{
		A7105_WriteReg(i, A7105Config[i]);
	}
}

void A7105_Cal(void)
{
	uint8_t tmp = 0;
	uint8_t fb, fbcf;
	uint8_t vb1, vbcf1, dvt1;
	uint8_t vb2, vbcf2, dvt2;

	StrobeCmd(CMD_STBY);	// calibration @ STB state

	// calibration IF Procedure
	A7105_WriteReg(CALIBRATION_REG, 0x01);
	WaitBit_0(CALIBRATION_REG, 0x01);

	// for check
	tmp = A7105_ReadReg(IFCAL1_REG);
	fb = tmp & 0x0F;
	fbcf = (tmp >> 4) & 0x01;

	if (fbcf == 1)
	{
		debuglog("calib IF fail \r\n");
		return;
	}

	// calibration vco procedure
	A7105_WriteReg(VCOCCAL_REG, 0x13);	// manual VCOC = 3
	A7105_WriteReg(VCOCAL2_REG, 0x3B);	// VTL = 3, VTH = 7

	SetCH(0);		// setting 2400MHz
	A7105_WriteReg(CALIBRATION_REG, 0x02);
	WaitBit_0(CALIBRATION_REG, 0x02);

	tmp = A7105_ReadReg(VCOCAL1_REG);
	vb1 = tmp & 0x07;
	vbcf1 = (tmp >> 3) & 0x01;
	dvt1 = (tmp >> 4) & 0x03;

	SetCH(160);	// setting 2480MHz
	A7105_WriteReg(CALIBRATION_REG, 0x02);
	WaitBit_0(CALIBRATION_REG, 0x02);

	tmp = A7105_ReadReg(VCOCAL1_REG);
	vb2 = tmp & 0x07;
	vbcf2 = (tmp >> 3) & 0x01;
	dvt2 = (tmp >> 4) & 0x03;

	SetIVCOBand(vb1, vb2);

	if (vbcf1 == 1 && vbcf2 == 1)
	{
		debuglog("calib vbcf1 vbcf2 == 1 \r\n");
		return;
	}


}

void A7105_RCO_Cal(void);

uint8_t A7105_CRC_OK(void)
{
	uint8_t value = A7105_ReadReg(MODE_REG) & (0x20);

	if (value == 0) {
		// crc ok
		return 1;
	} else {
		return 0;
	}

}

void RxPacket(uint8_t *data, int len)
{

	uint8_t cmd = 0x40 | FIFO_REG;

	RF_CS_LOW;

	HAL_SPI_Transmit(&hspi1, &cmd, 1, 2 * 1000);

	if (len > 64)
	{
		len = 64;
	}

	HAL_SPI_Receive(&hspi1, data, len, 2 * 1000);

	RF_CS_HI;
}

void StrobeCmd(uint8_t cmd)
{
	RF_CS_LOW;
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 2 * 1000);
	RF_CS_HI;

}

void SetCH(uint8_t ch)
{
	A7105_WriteReg(PLL1_REG, ch);
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
		A7105_WriteReg(VCOCAL1_REG, (vb1 | 0x08));
	}
	else
	{
		A7105_WriteReg(VCOCAL1_REG, (vb2 | 0x08));
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
