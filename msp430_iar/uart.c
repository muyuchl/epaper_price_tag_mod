#include <MSP430G2553.h>
#include "uart.h"

extern volatile unsigned char cmd;

void uart_init(void)
{
P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
       P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
       UCA0CTL1 |= UCSSEL_2;                     // SMCLK
       /*
       UCA0BR0 = 104;                            // 1MHz 9600
       UCA0BR1 = 0;                              // 1MHz 9600
       UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
      */
       UCA0BR0 = 0x08; // 1MHz 115200
   UCA0BR1 = 0x00; // 1MHz 115200
   UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5

       UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
       IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}


//  Echo back RXed character, confirm TX buffer is ready first
 #pragma vector=USCIAB0RX_VECTOR
 __interrupt void USCI0RX_ISR(void)
 {

  cmd = UCA0RXBUF;

  // while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  // UCA0TXBUF = UCA0RXBUF;                   // TX -> RXed character
 }

void uart_send_ch(unsigned char ch)
 {

     while(!(IFG2& UCA0TXIFG)); //查询发送是否结束
     UCA0TXBUF = ch;
     IFG2&=~UCA0TXIFG; //清除发送一标志位
 }
 
 //发送字符串
 void uart_send_str(const char *str)
 {
       for( ; *str ; )
       {
           uart_send_ch((unsigned char)*str);
           str++;
       }
 }

static const char hextable[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
} ;

 void uart_send_ch_hex(unsigned char ch)
 {

  unsigned char half = ch >> 4;
  uart_send_ch(hextable[half]);
  half = ch & 0x0F;
  uart_send_ch(hextable[half]);
 }