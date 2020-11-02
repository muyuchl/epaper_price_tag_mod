#include "rfcommand.h"
#include "a7105.h"
#include "flash.h"
#include "epd.h"

const int ONE_READ_SIZE = 16;

void check_and_handle_rf_command()
{

  uint8_t buf[RF_FRAME_LEN] = {0};
  a7105_read_rxdata(buf, RF_FRAME_LEN);

  switch (buf[0]) {
  	case RFCMD_PING:
  		handle_ping_cmd(buf);
  		break;
      case RFCMD_READ_FLASH_JEDECID:
      handle_flash_jedecid(buf);
      break;
      case RFCMD_READ_FLASH_DATA:
      handle_flash_read(buf);
      break;

case RFCMD_FLASH_SECTOR_ERASE:
      handle_sector_erase(buf);
      break;

      case RFCMD_FLASH_CHIP_ERASE:
      handle_chip_erase(buf);
      break;

      case RFCMD_WRITE_FLASH_DATA:
      handle_flash_write(buf);
      break;

      case RFCMD_EPAPER_FILL:
      handle_epd_fill(buf);
      break;

      case RFCMD_LOAD_IMAGE_FROM_FLASH:
      handle_epd_load_from_flash(buf);
      break;

  	default:
  	break;
  }

}

void handle_ping_cmd(unsigned char *buf)
{
	buf[0] = RFCMD_PING_RESP;

	a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_flash_jedecid()
{
  uint8_t buf[RF_FRAME_LEN] = {0};

  buf[0] = RFCMD_READ_FLASH_JEDECID_RESP;
  flash_read_id(buf + 1);

  a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_flash_read(unsigned char *buf)
{
buf[0] = RFCMD_READ_FLASH_DATA_RESP;

uint32_t addr = ((uint32_t)(buf[1]) << 16) + ((uint32_t)(buf[2]) << 8) +
                ((uint32_t)(buf[3]) );

uint16_t len =   ((uint16_t)(buf[4]) << 8) +   ((uint16_t)(buf[5]) );

  flash_read_data(buf + 6, addr, len);

a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_sector_erase(unsigned char *buf)
{
buf[0] = RFCMD_FLASH_SECTOR_ERASE_RESP;

uint32_t addr = ((uint32_t)(buf[1]) << 16) + ((uint32_t)(buf[2]) << 8) +
                ((uint32_t)(buf[3]) );


  flash_sector_erase(addr);

a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_chip_erase(unsigned char *buf)
{
buf[0] = RFCMD_FLASH_CHIP_ERASE_RESP;



  flash_chip_erase();

a7105_tx_data(buf, RF_FRAME_LEN);

}

void handle_flash_write(unsigned char *buf)
{
buf[0] = RFCMD_WRITE_FLASH_DATA_RESP;

uint32_t addr = ((uint32_t)(buf[1]) << 16) + ((uint32_t)(buf[2]) << 8) +
                ((uint32_t)(buf[3]) );

uint16_t len =   ((uint16_t)(buf[4]) << 8) +   ((uint16_t)(buf[5]) );

  flash_write_data(addr, buf+6, len);

a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_epd_fill(unsigned char *buf)
{
  buf[0] = RFCMD_EPAPER_FILL_RESP;

  uint8_t fillvalue = buf[1];
epd_fill(fillvalue);

a7105_tx_data(buf, RF_FRAME_LEN);
}

void handle_epd_load_from_flash(unsigned char *buf)
{
buf[0] = RFCMD_LOAD_IMAGE_FROM_FLASH_RESP;

uint32_t addr = ((uint32_t)(buf[1]) << 16) + ((uint32_t)(buf[2]) << 8) +
                ((uint32_t)(buf[3]) );


uint8_t img_buf[16];

epd_pre_update();



for (int i = 0; i < 4000 / ONE_READ_SIZE; i++) {
  flash_read_data(img_buf, addr + i * ONE_READ_SIZE, ONE_READ_SIZE);
  epd_update_ram(img_buf, ONE_READ_SIZE);
}


 epd_post_update();


a7105_tx_data(buf, RF_FRAME_LEN);

}