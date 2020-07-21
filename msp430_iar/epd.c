#include <MSP430G2553.h>
#include "epd.h"
//#include "imagedata.h"

static void sendbyte(int sdbyte);
static void epd_send_cmd(int cmd);
static void epd_sendcmddata(int cmd,int data);
static void Delay(unsigned int nCount);
static void Delaylong(unsigned int n10Count);

const unsigned char epdinit[];


/*
static void fill_predefinedimage()
{
  // set ram x address counter
  epd_sendcmddata(0x4e,0x0);  
// set ram y address counter
  epd_sendcmddata(0x4f,0x0);    
  epd_send_cmd(0x24);            //  write ram     
   epdcshigh;
   for(int s=0;s<4000;s++)
      { 
        epdcslow;    
        sendbyte(img1[s]);
        epdcshigh;
      }
      // display update control 2, C7: setting for LUT from MCU
    epd_sendcmddata(0x22,0xc7); 
    // Master Activation
    epd_send_cmd(0x20);    
}
*/

void epd_init()
{

  epdunknowhigh;
  epdbslow ;  
  epdon;
  
  // P1DIR = P2 | P3 | P4 | P6 | P7 ;
  P2DIR |= BIT3 | BIT4 | BIT5 | BIT6 ;
  P3DIR |=  BIT1 | BIT4 | BIT5 | BIT6 | BIT7 ;
  


  P1SEL = 0X0;
  P1SEL2 = 0X0;
  P2SEL = 0X0;
  P2SEL2 = 0X0;
  P3SEL = 0X0;
  P3SEL2 = 0X0; 

  Delaylong(3);
  epdrsthigh;
  Delaylong(3);
  epdrstlow;
  Delaylong(3);
  epdrsthigh;  
  Delaylong(3);


      unsigned char i;
        // sw reset
        epd_send_cmd(0x12);
        Delaylong(1);
        for ( i = 0; i <= 0x33; i++ )
        {
                if ( (i == 0x0) || (i == 0x3) || (i == 0x5) || (i == 0x7) || 
                    (i == 0x9) || (i == 0xc) || (i == 0xf) || (i == 0x11) ||
                    (i == 0x13) ||  (i == 0x32)) 
                {
                    // dc low is command
                  epddclow;
                }
              epdcslow;
              sendbyte(epdinit[i]);
              epdcshigh;
              epddchigh;
              asm("nop");
              asm("nop");
         }

    //     fill_predefinedimage();
}

void epd_pre_update()
{
  // set ram x address counter
  epd_sendcmddata(0x4e,0x0);  
// set ram y address counter
  epd_sendcmddata(0x4f,0x0);    
  epd_send_cmd(0x24);            //  write ram     
   epdcshigh;
}

void epd_update_ram(unsigned char *buf, int len)
{
  for (int i = 0; i < len; i++) {
    epdcslow;    
        sendbyte(buf[i]);
        epdcshigh;
  }
}

void epd_post_update()
{
    // display update control 2, C7: setting for LUT from MCU
    epd_sendcmddata(0x22,0xc7); 
    // Master Activation
    epd_send_cmd(0x20);    
}

void epd_fill(unsigned char val)
{

// set ram x address counter
  epd_sendcmddata(0x4e,0x0);  
// set ram y address counter
  epd_sendcmddata(0x4f,0x0);    
  epd_send_cmd(0x24);            //  write ram     
   epdcshigh;
   for(int s=0;s<4000;s++)
      { 
        epdcslow;    
        sendbyte(val);
        epdcshigh;
      }
      // display update control 2, C7: setting for LUT from MCU
    epd_sendcmddata(0x22,0xc7); 
    // Master Activation
    epd_send_cmd(0x20);     

}

/*
const unsigned char epdinit[]=
{
// idx is the index in epdinit array    
// idx 0, Driver Output Control 
0x01,0xf9,0x00,
// idx 3, Set Dummy line period  
0x3a,0x06,
// idx 5, Set Gate Line width
0x3b,0x0b,
// idx 7, Data Entry mode setting,   Y increment, X increment
0x11,0x03,
// idx 9,Set RAM X address Start / End position, 
0x44,0x00,0x0f,
// idx 0x0c Set RAM Y address Start / End position, 0xf9 is 249
0x45,0x00,0xf9,
// idx 0xf, Write VCOM register, 0xa0 ??
0x2c,0xa0,
// idx 0x11 Border Waveform Control, 
0x3c,0x33,
// idx 0x13, Write LUT register 
0x32,0x66,0x66,0x26,0x04,

0x55,0xaa,
0x08,0x91,0x11,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x19,0x0a,
0x0a,0x5e,0x1e,0x1e,0x0a,0x39,0x14,0x00,0x00,0x00,0x02,
// display update control,enable bypass
0x21,0x83,
 };
 */
extern const unsigned char epdinit[]=
{
0x01,0xf9,0x00,0x3a,0x06,0x3b,0x0b,0x11,0x03,0x44,0x00,0x0f,0x45,
0x00,0xf9,0x2c,0xa0,0x3c,0x33,0x32,0x66,0x66,0x26,0x04,0x55,0xaa,
0x08,0x91,0x11,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x19,0x0a,
0x0a,0x5e,0x1e,0x1e,0x0a,0x39,0x14,0x00,0x00,0x00,0x02,0x21,0x83,
 
 };

 static void sendbyte(int sdbyte)
 {
 	 volatile unsigned int i;
  for (i =0; i<8;i++)
  {
     epdcklow; 
    if (sdbyte & 0x80)
      { epddahigh;
        }
    else
      { epddalow;
       }
    sdbyte <<=1;
     epdckhigh;  
  }
  epdcklow;
 }

static void epd_send_cmd(int cmd)
{
 epddclow;
  epdcslow;
  sendbyte(cmd);
  epdcshigh;
  epddchigh;
}

static void epd_sendcmddata(int cmd,int data)
{
	 epddclow;
  epdcslow;
  sendbyte(cmd);
  epddchigh;
  sendbyte(data);
  epdcshigh;
}

static void Delay(unsigned int nCount)
{

	 for(;nCount!=0;nCount--)
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

static void Delaylong(unsigned int n10Count)
{
	  unsigned char i;
            while(n10Count--)
            {
              for(i = 0; i < 10; i ++)
                {
                        Delay(10000);
                }
            }
}

