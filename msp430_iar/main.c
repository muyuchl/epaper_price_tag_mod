
#include <MSP430G2553.h>
//#include "uart.h"
#include "a7105.h"
#include "flash.h"
#include "epd.h"
#include "rfcommand.h"
#include <stdint.h>

// p2.7 
#define rfon               P2OUT &= 0x7f
#define rfoff              P2OUT |= 0x80

volatile uint16_t tick = 0;
uint8_t low_power_state = 0;

// if idle more than this tick, enter low power mode
const uint16_t INITIAL_IDLE_TICK_COUNT = 5;
const uint16_t SWITCH_PIC_TICK_COUNT = 5;

int count = 0;
int crcErrCount = 0;

volatile unsigned char cmd = 0;

uint8_t write_flash_buf[8];

uint8_t buf[RF_FRAME_LEN];

const uint8_t random_table[64] = {
	11, 34, 47, 61, 39, 12, 37, 19, 32, 42, 48, 3, 60, 21, 22, 16, 62, 17, 38, 8, 57, 18, 
    25, 9, 23, 59, 29, 15, 56, 44, 40, 1, 63, 35, 27, 30, 2, 51, 33, 28, 58, 5, 14, 49,
     50, 54, 0, 46, 4, 26, 41, 53, 45, 20, 13, 6, 52, 10, 36, 31, 43, 24, 55, 7
 };

 uint8_t random_table_index = 0;

void delay_ms(int count)
{
  for (int i = 0; i < count; i++)
  {
    for (int j = 0; j < 160; j++)
      _NOP();
  }
}

static void check_rf();
static void switch_pic();

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  /* Use Calibration values for 1MHz Clock DCO*/
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ + DIVA_3;  // XTS is 0, ACLK divide 8
DCOCTL = CALDCO_1MHZ;

BCSCTL3 = LFXT1S_2; // VLO 12KHz

// alt name TACCTL0
CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TACTL = TASSEL_1 + MC_1 + ID_3;           // ACLK/8, upmode  
  // ACLK 12kHz/8 = 1500Hz,  TA0 clk = 1500/8 = 187.5
  CCR0 =  11250;   // 1 minute per interrupt

/*
uart_init();

uart_send_str("a7105 communication ");
uart_send_str(__DATE__);
uart_send_str(" ");
uart_send_str(__TIME__);
uart_send_str("\r\n");
*/

delay_ms(40);
epd_init();

// rf on off pin
P2DIR |= BIT7;

rfon;
delay_ms(400);

a7105_init();
a7105_toRxMode();
flash_init();

 __bis_SR_register( GIE); //  interrupts enabled
  
// display_menu();

  while(1)
  {
  	if (low_power_state) {
  		if (tick >= SWITCH_PIC_TICK_COUNT) {
  			tick = 0; // reset to 0
  			// switch picture
  			switch_pic();
  			delay_ms(400);
  			
  		}

  		LPM3;

  	} else {
  		check_rf();		

  		if (tick >= INITIAL_IDLE_TICK_COUNT) {
  			// enter low power state
  			low_power_state = 1;
  			tick = 0;	// reset to 0

  		}
  	}
            
  }

 // return 0;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  tick++;

  if (low_power_state
  	&& tick >= SWITCH_PIC_TICK_COUNT) {
  		LPM3_EXIT;
  }
  // 
}

static void check_rf()
{
if (a7105_gio2_low() ) {

    	if (a7105_rx_crc_ok()) {
    		check_and_handle_rf_command();

    		// reset to 0, so not entering low power mode
    		tick = 0;
    	} else {
    		crcErrCount++;
    		delay_ms(100);
    		// read to discard
    		a7105_read_rxdata(buf, RF_FRAME_LEN);
    		delay_ms(10);
    		a7105_strobe_cmd(CMD_STBY);
    		delay_ms(10);
    	}

    	// back to rx mode
    	a7105_toRxMode();
    } 

}

static void switch_pic()
{
	const uint16_t ONE_READ_SIZE = 16;

	uint8_t sector_index = 0;

	random_table_index++;
	random_table_index = random_table_index % 64;

	sector_index = random_table[sector_index];
	// redundunt
	sector_index = sector_index % 64;


	uint32_t addr = 4096 * sector_index;


uint8_t img_buf[16];

epd_pre_update();

for (int i = 0; i < 4000 / ONE_READ_SIZE; i++) {
  flash_read_data(img_buf, addr + i * ONE_READ_SIZE, ONE_READ_SIZE);
  epd_update_ram(img_buf, ONE_READ_SIZE);
}


 epd_post_update();


}