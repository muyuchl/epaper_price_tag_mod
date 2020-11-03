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

    // read jedec id,
    // response:
    // cmd status(1 byte), 0 for ok
    // manufacturer (1 byte), 0xEF, winbond
    // storage type (1 byte),
    // capacity ( 1byte)
    CMD_FLASH_READ_JEDEC_ID = 0x40,

    // request:
    //  cmd  1 byte
    //  addr A23-A16
    // addr A15-A8
    // addr A7-A0
    //   bytes to read (MSB)
    // bytes to read (LSB)

    // response:
    // cmd code
    // cmd exec status, 0 for ok
    // data (same size as requested
    CMD_FLASH_READ_DATA = 0x41,

    // request:
    //  cmd  1 byte
    //  addr A23-A16
    // addr A15-A8
    // addr A7-A0
    //   bytes to write (MSB)
    // bytes to write (LSB)
    // data

    // response:
    // cmd code
    // cmd exec status, 0 for ok
    //
    CMD_FLASH_WRITE_DATA = 0x42,

    // request:
    //  cmd  1 byte
    //  addr A23-A16
    // addr A15-A8
    // addr A7-A0
    // response:
    // cmd code
    // cmd exec status, 0 for ok
    //
    CMD_FLASH_SECTOR_ERASE = 0x43,

};

// rf frame is embedded in uart frame, it has its own command
enum {
    RFCMD_RESERVE = 0x00,
    // ping 4 byte data follow command
    RFCMD_PING = 0x01,
    // response with 4 byte data received
    RFCMD_PING_RESP = 0x02,

    RFCMD_READ_FLASH_JEDECID = 0x03,
    // response with 3 byte id
    RFCMD_READ_FLASH_JEDECID_RESP = 0x04,

    // 3 byte address (MSB first)
    // 2 byte length (msb first)
    RFCMD_READ_FLASH_DATA = 0x05,
    // 3 byte address (MSB first)
    // 2 byte length (msb first)
    // data bytes
    RFCMD_READ_FLASH_DATA_RESP = 0x06,

    // 3 byte address (MSB first)
    RFCMD_FLASH_SECTOR_ERASE = 0x07,
    // 3 byte address (MSB first)
    RFCMD_FLASH_SECTOR_ERASE_RESP = 0x08,

    // 3 byte address (MSB first)
    // 2 byte length (msb first)
    // data bytes
    RFCMD_WRITE_FLASH_DATA = 0x09,
    // 3 byte address (MSB first)
    // 2 byte length (msb first)
    RFCMD_WRITE_FLASH_DATA_RESP = 0x09,

    // 1 byte fill data
    RFCMD_EPAPER_FILL = 0x0A,

    RFCMD_EPAPER_FILL_RESP = 0x0B,


    // 3 byte address (MSB first)
    RFCMD_LOAD_IMAGE_FROM_FLASH = 0x0C,
    // 3 byte address (MSB first)
    RFCMD_LOAD_IMAGE_FROM_FLASH_RESP = 0x0D,

    RFCMD_EPD_ON = 0x10,
    RFCMD_EPD_ON_RESP = 0x11,

    RFCMD_EPD_OFF = 0x12,
    RFCMD_EPD_OFF_RESP = 0x13,

    RFCMD_FLASH_CHIP_ERASE = 0x14,
    RFCMD_FLASH_CHIP_ERASE_RESP = 0x15,

    // 1 BYTE index data
    RFCMD_SET_IMAGE_RAND_INDEX = 0x16,
    RFCMD_SET_IMAGE_RAND_INDEX_RESP = 0x17,

};


#endif // COMMAND_H
