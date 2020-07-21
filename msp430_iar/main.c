
#include <MSP430G2553.h>
//#include "uart.h"
#include "a7105.h"
#include "flash.h"
#include "epd.h"
#include "rfcommand.h"

// p2.7 
#define rfon               P2OUT &= 0x7f
#define rfoff              P2OUT |= 0x80

int count = 0;

volatile unsigned char cmd = 0;

uint8_t write_flash_buf[8];

void delay_ms(int count)
{
  for (int i = 0; i < count; i++)
  {
    for (int j = 0; j < 160; j++)
      _NOP();
  }
}



int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  /* Use Calibration values for 1MHz Clock DCO*/
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ;
DCOCTL = CALDCO_1MHZ;

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

// __bis_SR_register( GIE); //  interrupts enabled
  
// display_menu();

  while(1)
  {
  	/*
    if (cmd != 0) {
      handle_uart_cmd();
    }
    */
   
    if (a7105_gio2_low() 
      	&& a7105_rx_crc_ok()) {
        
        check_and_handle_rf_command();

      // back to rx mode
    	a7105_toRxMode();
       // delay_ms(100);
    }
  }

 // return 0;
}
