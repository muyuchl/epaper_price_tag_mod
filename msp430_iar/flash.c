#include "flash.h"

#include <stdint.h>

#define CMD_READ_DEVICE_ID 0x9f

#define CMD_PAGE_PROGRAM 0x02

#define CMD_READ_DATA 0x03
#define CMD_READ_STATUS_REGISTER 0x05

#define CMD_WRITE_ENABLE 0x06
#define CMD_SECTOR_ERASE 0x20

#define CMD_CHIP_ERASE 0xC7

#define FLASH_BUSY_FLAG 0x01

#ifdef PRICETAG_DEVICE
//SPI nCS:      P3.0  430 output
#define CS_1 P3OUT |= BIT0
#define CS_0 P3OUT &= ~BIT0


//SPI CLK:      P1.5  430 output
#define CLK_1 P1OUT |= BIT5
#define CLK_0 P1OUT &= ~BIT5
 
 
//SPI MOSI:P1.7     430 output, flash di
#define DIO_1 P1OUT |= BIT7
#define DIO_0 P1OUT &= ~BIT7

// flash do,  MISO  P1.6   430 input
#define DO_IN P1IN & BIT6

#else
//SPI nCS:      P2.2  430 output
#define CS_1 P2OUT |= BIT2
#define CS_0 P2OUT &= ~BIT2


//SPI CLK:      P2.4  430 output
#define CLK_1 P2OUT |= BIT4
#define CLK_0 P2OUT &= ~BIT4
 
 
//SPI MOSI:P2.3     430 output, flash di
#define DIO_1 P2OUT |= BIT3
#define DIO_0 P2OUT &= ~BIT3

// flash do,  MISO  P2.0   430 input
#define DO_IN P2IN & BIT0
#endif


static void delay_us( unsigned int k )
{
    for (int j = 0; j < 5 * k; j++)
    {
      _NOP();
    }
  
}



void flash_init()
{
#ifdef PRICETAG_DEVICE
	// output
	P1DIR |= BIT5  + BIT7;
	P3DIR |= BIT0;
	// input
	P1DIR &= ~BIT6;

#else
	// output
	P2DIR |= BIT2 + BIT3 + BIT4;
	// input
	P2DIR &= ~BIT0;
#endif

	CS_1;
	CLK_0;

	delay_us(10);
}

void flash_send_byte(unsigned char data)
{
// mode 0

	CLK_0;
	delay_us(2);

	for (int i = 0; i < 8; i++)
	{
		CLK_0;

		if (data & 0x80) {
			DIO_1;
		} else {
			DIO_0;
		}

		delay_us(5);
		
		CLK_1;
		delay_us(2);

		data = data << 1;
		
	}

	CLK_0;
	delay_us(2);
}

unsigned char flash_read_byte()
{
	unsigned char data = 0;

	CLK_0;
	delay_us(2);

	for (int i = 0; i < 8; i++) {
		data <<= 1;

		CLK_1;
		delay_us(2);

		if (DO_IN) {
			data |=  0x01;
		}
		
		
		CLK_0;
		delay_us(2);
	}

	CLK_0;
		delay_us(2);

	return data;
}

void wait_until_not_busy()
{
	uint8_t txbuf[4]  = {0};
	uint8_t status = 0;

	CS_0;

	txbuf[0] = CMD_READ_STATUS_REGISTER;

	flash_send_byte(txbuf[0]);

	status = flash_read_byte();

	
	while ( (status & FLASH_BUSY_FLAG) != 0 ) {
		delay_us(20);
		status = flash_read_byte();
	}

	CS_1;
}

void flash_write_enable()
{
	uint8_t txbuf[4]  = {0};

	CS_0;

	txbuf[0] = CMD_WRITE_ENABLE;
	
	flash_send_byte(txbuf[0]);

	CS_1;
}

void flash_read_id(uint8_t *buf)
{
	uint8_t txbuf[1]  = {0};
	
	CS_0;

	txbuf[0] = CMD_READ_DEVICE_ID;       //

	flash_send_byte(txbuf[0]);

	delay_us(6);

	for (int i = 0; i < 3; i++) {
		buf[i] = flash_read_byte();
		delay_us(6);
	}

	CS_1;
}

void flash_read_data(uint8_t *buf, uint32_t addr, uint16_t len)
{
	uint8_t txbuf[4]  = {0};

	CS_0;

	txbuf[0] = CMD_READ_DATA;
	txbuf[1] = (uint8_t)(addr >> 16);
	txbuf[2] =  (uint8_t)(addr >> 8);
	txbuf[3] =  (uint8_t)(addr >> 0);

	for (int i = 0; i < 4; i++) {
		flash_send_byte(txbuf[i]);
	}

	for (int i = 0; i < len; i++) {
		buf[i] = flash_read_byte();
	}


	CS_1;
}

void flash_sector_erase(uint32_t addr)
{
	flash_write_enable();
	wait_until_not_busy();

	uint8_t txbuf[4]  = {0};

	CS_0;

	txbuf[0] = CMD_SECTOR_ERASE;
	txbuf[1] = (uint8_t)(addr >> 16);
	txbuf[2] =  (uint8_t)(addr >> 8);
	txbuf[3] =  (uint8_t)(addr >> 0);

	for (int i = 0; i < 4; i++) {
		flash_send_byte(txbuf[i]);
	}
	

	CS_1;

	wait_until_not_busy();
}

void flash_chip_erase()
{
flash_write_enable();
	wait_until_not_busy();

	uint8_t txbuf[4]  = {0};

	CS_0;

	txbuf[0] = CMD_CHIP_ERASE;
	

	
	flash_send_byte(txbuf[0]);
	
	

	CS_1;

	wait_until_not_busy();
}

/* workaround, len is less than or equal 8 byte, otherwise not reliable
but according to datasheet, one write 256 byte is not a problem.
maybe it's issue about timging */
void do_write_data(uint32_t addr, uint8_t *buf, uint16_t len)
{
	flash_write_enable();

	uint8_t txbuf[4]  = {0};

	CS_0;

	txbuf[0] = CMD_PAGE_PROGRAM;
	txbuf[1] = (uint8_t)(addr >> 16);
	txbuf[2] =  (uint8_t)(addr >> 8);
	txbuf[3] =  (uint8_t)(addr >> 0);

	for (int i = 0; i < 4; i++) {
		flash_send_byte(txbuf[i]);
	}

	for (int i = 0; i < len; i++) {
		flash_send_byte(buf[i]);
	}
	

	CS_1;

	wait_until_not_busy();
}

void flash_write_data(uint32_t addr, uint8_t *buf, uint16_t len)
{  
	int count = 0;

	count = len / 8;
    	for (int i = 0; i < count; i++) {
    		int offset = i * 8;
    		do_write_data(addr + offset, buf+offset, 8);
    		delay_us(6);	
    	}

    
    if (0 != (len % 8)) {
    	// last segment, less than 8 byte
    	do_write_data(addr + count * 8, buf + count * 8, len % 8);	
    } 
}