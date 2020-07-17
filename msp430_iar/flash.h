#ifndef FLASH_H
#define FLASH_H

#include <MSP430G2553.h>

#include <stdint.h>

#include "config.h"

// #define Set_Bit(port, pin)  (port |= pin)
// #define Clr_Bit(port, pin)  (port &= ~pin)
// #define Get_Bit(port, pin)  ((port & pin) != 0)

      


void flash_init();


void flash_send_byte(unsigned char data);
unsigned char flash_read_byte();

void flash_read_id(uint8_t *buf);

void flash_read_data(uint8_t *buf, uint32_t addr, uint16_t len);

void flash_sector_erase(uint32_t addr);

void flash_write_data(uint32_t addr, uint8_t *buf, uint16_t len);

#endif