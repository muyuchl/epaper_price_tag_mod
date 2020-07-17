// e-ink paper display
#ifndef EPD_H
#define EPD_H
// p2.3
#define epdcklow           P2OUT &= 0xf7
#define epdckhigh          P2OUT |= 0x08

// p2.4
#define epddalow           P2OUT &= 0xef
#define epddahigh          P2OUT |= 0x10

// p3.4
#define epdcslow           P3OUT &= 0xef
#define epdcshigh          P3OUT |= 0x10

// p3.5
#define epddclow           P3OUT &= 0xdf
#define epddchigh          P3OUT |= 0x20

// p3.6
#define epdrstlow          P3OUT &= 0xbf
#define epdrsthigh         P3OUT |= 0x40

// p3.1
#define epdbslow          P3OUT &= 0xfd

// unknown p3.7
#define epdunknowhigh          P3OUT &= 0x80

// power on off, p2.6, L to power on
#define epdon           P2OUT &= 0xbf
#define epdoff          P2OUT |= 0x40

void epd_init();

void epd_fill(unsigned char val);

void epd_pre_update();
void epd_update_ram(unsigned char *buf, int len);
void epd_post_update();

#endif