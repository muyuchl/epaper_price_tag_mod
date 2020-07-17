#ifndef A7105_H
#define A7105_H

#include <MSP430G2553.h>
#include <stdint.h>
#include "config.h"

#define RF_FRAME_LEN 64

#define Set_Bit(port, pin)  (port |= pin)
#define Clr_Bit(port, pin)  (port &= ~pin)
#define Get_Bit(port, pin)  ((port & pin) != 0)

#ifdef PRICETAG_DEVICE
//SPI nCS:      P1.3  430 output
#define        SCS_DIR        (P1DIR)
#define        SCS_OUT        (P1OUT)
#define        SCS_REN        (P1REN)
#define        SCS_PIN        (BIT3)
 
#define        SCS_UP            (Set_Bit(SCS_REN,SCS_PIN))
#define        SCS_DirOut        (Set_Bit(SCS_DIR,SCS_PIN))
#define        SCS_H            (Set_Bit(SCS_OUT,SCS_PIN))
#define        SCS_L            (Clr_Bit(SCS_OUT,SCS_PIN))

//SPI CLK:      P1.4  430 output
#define        SCLK_DIR        (P1DIR)
#define        SCLK_OUT        (P1OUT)
#define        SCLK_REN        (P1REN)
#define        SCLK_PIN        (BIT4)
 
#define        SCLK_UP            (Set_Bit(SCLK_REN,SCLK_PIN))
#define        SCLK_DirOut        (Set_Bit(SCLK_DIR,SCLK_PIN))
#define        SCLK_H            (Set_Bit(SCLK_OUT,SCLK_PIN))
#define        SCLK_L            (Clr_Bit(SCLK_OUT,SCLK_PIN))
 
 
//SPI DA MOSI:P1.2     430 output
#define        SDIO_DIR            (P1DIR)
#define        SDIO_OUT            (P1OUT)
//#define        SDIO_IN            (P1IN)
#define        SDIO_REN            (P1REN)
#define        SDIO_PIN            (BIT2)
 
#define        SDIO_UP            (Set_Bit(SDIO_REN,SDIO_PIN))
#define        SDIO_DirOut        (Set_Bit(SDIO_DIR,SDIO_PIN))
#define        SDIO_H            (Set_Bit(SDIO_OUT,SDIO_PIN))
#define        SDIO_L            (Clr_Bit(SDIO_OUT,SDIO_PIN))

// GIO1 MISO  P1.1   430 input
#define        GIO1_DIR            (P1DIR)
#define        GIO1_OUT            (P1OUT)
#define        GIO1_IN            (P1IN)
#define        GIO1_REN            (P1REN)
#define        GIO1_PIN            (BIT1)

#define        GIO1_DirIn        (Clr_Bit(GIO1_DIR,GIO1_PIN))
#define        GIO1_Data         (Get_Bit(GIO1_IN,GIO1_PIN))

#define        GIO1_PULLUP     do {	\
									GIO1_REN |= GIO1_PIN; \
									GIO1_OUT |= GIO1_PIN;\
										} while (0);
      

// GIO2  P1.0   430 input
#define        GIO2_DIR            (P1DIR)
#define        GIO2_OUT            (P1OUT)
#define        GIO2_IN            (P1IN)
#define        GIO2_REN            (P1REN)
#define        GIO2_PIN            (BIT0)

#define        GIO2_DirIn         (Clr_Bit(GIO2_DIR,GIO2_PIN))
#define        GIO2_Data          (Get_Bit(GIO2_IN,GIO2_PIN))

#define        GIO2_PULLUP            do {	\
									GIO2_REN |= GIO2_PIN; \
									GIO2_OUT |= GIO2_PIN;\
										} while (0);

#else
// not on price tag device
//SPI nCS:      P2.5  430 output
#define        SCS_DIR        (P2DIR)
#define        SCS_OUT        (P2OUT)
#define        SCS_REN        (P2REN)
#define        SCS_PIN        (BIT5)
 
#define        SCS_UP            (Set_Bit(SCS_REN,SCS_PIN))
#define        SCS_DirOut        (Set_Bit(SCS_DIR,SCS_PIN))
#define        SCS_H            (Set_Bit(SCS_OUT,SCS_PIN))
#define        SCS_L            (Clr_Bit(SCS_OUT,SCS_PIN))

//SPI CLK:      P2.4  430 output
#define        SCLK_DIR        (P2DIR)
#define        SCLK_OUT        (P2OUT)
#define        SCLK_REN        (P2REN)
#define        SCLK_PIN        (BIT4)
 
#define        SCLK_UP            (Set_Bit(SCLK_REN,SCLK_PIN))
#define        SCLK_DirOut        (Set_Bit(SCLK_DIR,SCLK_PIN))
#define        SCLK_H            (Set_Bit(SCLK_OUT,SCLK_PIN))
#define        SCLK_L            (Clr_Bit(SCLK_OUT,SCLK_PIN))
 
 
//SPI MOSI:P2.3     430 output
#define        SDIO_DIR            (P2DIR)
#define        SDIO_OUT            (P2OUT)
//#define        SDIO_IN            (P2IN)
#define        SDIO_REN            (P2REN)
#define        SDIO_PIN            (BIT3)
 
#define        SDIO_UP            (Set_Bit(SDIO_REN,SDIO_PIN))
#define        SDIO_DirOut        (Set_Bit(SDIO_DIR,SDIO_PIN))
#define        SDIO_H            (Set_Bit(SDIO_OUT,SDIO_PIN))
#define        SDIO_L            (Clr_Bit(SDIO_OUT,SDIO_PIN))

// GIO1 MISO  P2.0   430 input
#define        GIO1_DIR            (P2DIR)
#define        GIO1_OUT            (P2OUT)
#define        GIO1_IN            (P2IN)
#define        GIO1_REN            (P2REN)
#define        GIO1_PIN            (BIT0)

#define        GIO1_DirIn        (Clr_Bit(GIO1_DIR,GIO1_PIN))
#define        GIO1_Data         (Get_Bit(GIO1_IN,GIO1_PIN))

#define        GIO1_PULLUP     do {	\
									GIO1_REN |= GIO1_PIN; \
									GIO1_OUT |= GIO1_PIN;\
										} while (0);
      

// GIO2  P2.1   430 input
#define        GIO2_DIR            (P2DIR)
#define        GIO2_OUT            (P2OUT)
#define        GIO2_IN            (P2IN)
#define        GIO2_REN            (P2REN)
#define        GIO2_PIN            (BIT1)

#define        GIO2_DirIn         (Clr_Bit(GIO2_DIR,GIO2_PIN))
#define        GIO2_Data          (Get_Bit(GIO2_IN,GIO2_PIN))

#define        GIO2_PULLUP            do {	\
									GIO2_REN |= GIO2_PIN; \
									GIO2_OUT |= GIO2_PIN;\
										} while (0);


#endif										

// txing data gio2 hi if write it in process
// rx mode, is low when data received
#define A7105_BUSY (GIO2_IN & GIO2_PIN)

#define A7105_GIO2_IS_LOW ( (GIO2_IN & GIO2_PIN) == 0 )


void a7105_init();

void a7105_reset();

void a7105_config();
void a7105_cal();
//void a7105_RCO_cal();
void a7105_set_ch(uint8_t ch);
void a7105_toRxMode();

void a7105_strobe_cmd(uint8_t cmd);


uint8_t a7105_gio2_low();
uint8_t a7105_rx_crc_ok();


void a7105_read_rxdata(uint8_t *data, int len);

void a7105_tx_data(uint8_t *data, int len);

// internal
void a7105_writefifo(uint8_t *data, int len);



void a7105_write_reg(const unsigned char reg, unsigned char value);
unsigned char a7105_read_reg(const unsigned char reg);

void a7105_write_id(const unsigned char *id_data);
void a7105_read_id(unsigned char *id_data);

void a7105_send_byte(unsigned char data);
unsigned char a7105_read_byte();



//===================================================================================
// A7106 register address
//===================================================================================
#define MODE_REG            0x00
#define MODE_CONTROL_REG    0x01
#define CALIBRATION_REG     0x02
#define FIFO1_REG           0x03
#define FIFO2_REG           0x04
#define FIFO_REG            0x05
#define IDCODE_REG          0x06
#define RCOSC1_REG          0x07
#define RCOSC2_REG          0x08
#define RCOSC3_REG          0x09
#define CKO_REG             0x0A
#define GIO1_REG            0x0B
#define GIO2_REG            0x0C
#define CLOCK_REG           0x0D
#define DATARATE_REG        0x0E
#define PLL1_REG            0x0F
#define PLL2_REG            0x10
#define PLL3_REG            0x11
#define PLL4_REG            0x12
#define PLL5_REG            0x13
#define TX1_REG             0x14
#define TX2_REG             0x15
#define DELAY1_REG          0x16
#define DELAY2_REG          0x17
#define RX_REG              0x18
#define RXGAIN1_REG         0x19
#define RXGAIN2_REG         0x1A
#define RXGAIN3_REG         0x1B
#define RXGAIN4_REG         0x1C
#define RSSI_REG            0x1D
#define ADC_REG             0x1E
#define CODE1_REG           0x1F
#define CODE2_REG           0x20
#define CODE3_REG           0x21
#define IFCAL1_REG          0x22
#define IFCAL2_REG          0x23
#define VCOCCAL_REG         0x24
#define VCOCAL1_REG         0x25
#define VCOCAL2_REG         0x26
#define BATTERY_REG         0x27
#define TXTEST_REG          0x28
#define RXDEM1_REG          0x29
#define RXDEM2_REG          0x2A
#define CPC_REG             0x2B
#define CRYSTALTEST_REG     0x2C
#define PLLTEST_REG         0x2D
#define VCOTEST1_REG        0x2E
#define VCOTEST2_REG        0x2F
#define IFAT_REG            0x30
#define RSCALE_REG          0x31
#define FILTERTEST_REG      0x32


//===================================================================================
// A7106 strobe command
//===================================================================================
#define CMD_SLEEP           0x80    //1000,xxxx SLEEP mode
#define CMD_IDLE            0x90    //1001,xxxx IDLE mode
#define CMD_STBY            0xA0    //1010,xxxx Standby mode
#define CMD_PLL             0xB0    //1011,xxxx PLL mode
#define CMD_RX              0xC0    //1100,xxxx RX mode
#define CMD_TX              0xD0    //1101,xxxx TX mode
#define CMD_TFR             0xE0    //1110,xxxx TX FIFO reset
#define CMD_RFR             0xF0    //1111,xxxx RX FIFO reset

#endif