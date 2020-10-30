#ifndef COMMAND_H
#define COMMAND_H

#include "stdint.h"


enum
{
    CMD_PING = 1,

    CMD_LED_ON = 0x11,
    CMD_LED_OFF = 0x12,


    CMD_READ_EEPROM_BYTE = 0x20,
    CMD_WRITE_EEPROM_BYTE = 0x21,



    CMD_RF_TEST = 0x31,

    CMD_RF_TXDATA = 0x32,
    // initialed by MCU, when received RF data
    CMD_RF_DATARECEIVED = 0x33,

	CMD_RF_READ_ID = 0x34,
};



#endif // COMMAND_H
