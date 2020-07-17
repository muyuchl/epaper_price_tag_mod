#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_send_ch(unsigned char ch);
void uart_send_str(const char *str);

void uart_send_ch_hex(unsigned char ch);

#endif