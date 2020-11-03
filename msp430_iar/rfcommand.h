#ifndef RFCOMMAND_H
#define RFCOMMAND_H

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

    // 1 BYTE index data
    RFCMD_SET_IMAGE_RAND_INDEX = 0x10,
    RFCMD_SET_IMAGE_RAND_INDEX_RESP = 0x12,

    RFCMD_FLASH_CHIP_ERASE = 0x14,
    RFCMD_FLASH_CHIP_ERASE_RESP = 0x15,
};

void check_and_handle_rf_command();

void handle_ping_cmd(unsigned char *buf);
void handle_flash_jedecid();
void handle_flash_read(unsigned char *buf);

void handle_sector_erase(unsigned char *buf);
void handle_chip_erase(unsigned char *buf);

void handle_flash_write(unsigned char *buf);


void handle_epd_fill(unsigned char *buf);

void handle_epd_load_from_flash(unsigned char *buf);
void handle_set_image_rand_index(unsigned char *buf);

#endif