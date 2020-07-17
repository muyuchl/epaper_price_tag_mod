#include "uarttask.h"
#include "command.h"
#include "main.h"
#include "logdebug.h"

#include <stdio.h>
#include "a7105.h"
#include "a7105reg.h"

const uint8_t START_DELIMITER = 0x7E;
const uint16_t MAX_FRAME_LEN = 1024;

uint8_t tx_buf[128];

const uint8_t RESPONSE_FLAG = 0x80;

extern UART_HandleTypeDef huart1;
extern uint8_t connected_with_host;


void handle_command(uint8_t cmd, const char *cmd_data, int cmd_data_len);
void handle_command_ping(const char *cmd_data, int cmd_data_len);
void handle_command_ledon( const char *cmd_data, int cmd_data_len);
void handle_command_ledoff( const char *cmd_data, int cmd_data_len);
void handle_command_rftest( const char *cmd_data, int cmd_data_len);

void handle_command_rftx( const char *cmd_data, int cmd_data_len);

/* parse raw uart frame and handle commands */
void parse_rx_buf(const char *buf, int len)
{
	uint8_t cksum = 0;
	uint16_t frame_len = 0;

	if (len < 5)
	{
		// invalid
		return;
	}

	if (buf[0] != START_DELIMITER)
	{
		// invalid
		return;
	}

	// signed /unsigned conversion? need to check
	frame_len = (buf[1] << 8) + buf[2];
	if (frame_len > MAX_FRAME_LEN)
	{
		// invalid
		return;
	}

	if (frame_len + 4 < len)
	{
		// invalid
		return;
	}

	for (int i = 0; i < frame_len + 1; i++)
	{
		cksum += buf[3+i];
	}
	if (0xFF != cksum)
	{
		// invalid
		return;
	}

	handle_command(buf[3], buf+4, frame_len - 1);

}

/* assemble uart frame and send */
void tx_response(const char *buf, int len)
{
	uint8_t cksum = 0;

	if (len > 128)
	{
		// invalid
		return;
	}

	tx_buf[0] = START_DELIMITER;
	tx_buf[1] = ((uint16_t)len) >> 8;
	tx_buf[2] = (uint16_t)len & 0xFF;

	for (int i = 0; i < len; i++)
	{
		tx_buf[3 + i] = buf[i];
		cksum += buf[i];
	}

	cksum = 0xFF - cksum;

	tx_buf[3 + len] = cksum;

	HAL_UART_Transmit(&huart1,tx_buf, 4+len, 5000);
}

void report_rf_data(const uint8_t * buf, int len)
{
	char newbuf[65] = {0};
	if (len > 64) {
		len = 64;
	}

	newbuf[0] = (char)(CMD_RF_DATARECEIVED);

	for (int  i = 0; i < len; i++) {
		newbuf[i + 1] = buf[i];
	}

	tx_response(newbuf, len + 1);
}

void handle_command(uint8_t cmd, const char *cmd_data, int cmd_data_len)
{
	//	int responseFrameLen = 1;   // include response cmd
	//	char tmpBuf[64];
	//	tmpBuf[0] = RESPONSE_FLAG | cmd;

	switch (cmd)
	{
	case CMD_PING:
		handle_command_ping(cmd_data, cmd_data_len);
		break;
	case CMD_LED_ON:
		handle_command_ledon(cmd_data, cmd_data_len);
		break;
	case CMD_LED_OFF:
		handle_command_ledoff(cmd_data, cmd_data_len);
		break;
	case CMD_RF_TEST:
		handle_command_rftest(cmd_data, cmd_data_len);
		break;
	case CMD_RF_TXDATA:
		handle_command_rftx(cmd_data, cmd_data_len);
		break;
	}

}


void handle_command_ping(const char *cmd_data, int cmd_data_len)
{
	int responseFrameLen = 5;   // include response cmd
	char tmpBuf[64];

	tmpBuf[0] = RESPONSE_FLAG | CMD_PING;

	for (int i = 0; i < 4; i++)
	{
		tmpBuf[1 + i] = cmd_data[i];
	}
	responseFrameLen = 5;

	tx_response(tmpBuf, responseFrameLen);
}

void handle_command_ledon(const char *cmd_data, int cmd_data_len)
{
	int responseFrameLen = 1;   // include response cmd
	char tmpBuf[64];

	tmpBuf[0] = RESPONSE_FLAG | CMD_LED_ON;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	tx_response(tmpBuf, responseFrameLen);
}

void handle_command_ledoff(const char *cmd_data, int cmd_data_len)
{
	int responseFrameLen = 1;   // include response cmd
	char tmpBuf[64];

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	tmpBuf[0] = RESPONSE_FLAG | CMD_LED_OFF;

	tx_response(tmpBuf, responseFrameLen);
}

#if 0
void a7105_write_register(uint8_t address, uint8_t value)
{
	uint8_t buf[4];

	buf[0] = address;
	buf[1] = value;

	HAL_SPI_Transmit(&hspi1, buf, 2, 2 * 1000);
}

void a7105_reset()
{
	RF_CS_LOW;
	a7105_write_register(0x00, 0x00);
	RF_CS_HI;
}

void a7105_config()
{
	RF_CS_LOW;
	// GIO1 to 4 wire spi data out
	a7105_write_register(0x0b, 0x19);
	RF_CS_HI;
}


void a7105_writeid()
{
	uint8_t buf[5];

	buf[0] = 0x06;	// reg addr
	buf[1] = 0x11;	// id data byte 0
	buf[2] = 0x22;
	buf[3] = 0x33;
	buf[4] = 0x44;	// id data byte 3

	RF_CS_LOW;

	HAL_SPI_Transmit(&hspi1, buf, 5, 2 * 1000);

	RF_CS_HI;
}

void a7105_readid()
{
	debuglog("config rf chip \r\n");


	uint8_t buf[8] = {0};
	uint8_t rxbuf[8] = {0};

	RF_CS_LOW;

	buf[0] = 0x40 | 0x06;		// id register
	HAL_SPI_TransmitReceive(&hspi1, buf, rxbuf, 5, 2 * 1000);

	RF_CS_HI;


	char cbuf[60];
	int slen = sprintf(cbuf, "read id:  %02X,  %02X %02X %02X %02X \r\n", rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4]);

	debuglog(cbuf);
}

void a7105_read_1f_reg()
{
	debuglog("config rf chip \r\n");


	uint8_t buf[8] = {0};
	uint8_t rxbuf[8] = {0};

	RF_CS_LOW;

	buf[0] = 0x40 | 0x1F;		// id register

	HAL_SPI_Transmit(&hspi1, buf, 1, 2 * 1000);

	HAL_SPI_Receive(&hspi1, rxbuf, 1, 2 * 1000);

	RF_CS_HI;


	char cbuf[60];
	int slen = sprintf(cbuf, "read 1f register :  %02X,   \r\n", rxbuf[0]);

	debuglog(cbuf);

}
#endif

void handle_command_rftest(const char *cmd_data, int cmd_data_len)
{
	int responseFrameLen = 1;   // include response cmd
	char tmpBuf[4];


	//	uint8_t spibuf[8];
	//	char chbuf[64];

	tmpBuf[0] = RESPONSE_FLAG | CMD_RF_TEST;

	debuglog("testing rf chip \r\n");


	char rfbuf[64] = {0};
	for (int i = 0; i < 16; i++) {
		rfbuf[i] = 10 + i;
	}

	StrobeCmd(CMD_STBY);

	A7105_WriteFIFO((uint8_t *)rfbuf, 64);

	SetCH(RF_CHANNEL);

	StrobeCmd(CMD_TX);
	while (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
	{
		;
	}

	debuglog("data sent \r\n");
	// back to rx mode

	SetCH(RF_CHANNEL - 1);
	StrobeCmd(CMD_RX);


	tx_response(tmpBuf, responseFrameLen);
}

void handle_command_rftx(const char *cmd_data, int cmd_data_len)
{
	// set to true
	connected_with_host = 1;

	int responseFrameLen = 1;   // include response cmd
	char tmpBuf[4];


	//	uint8_t spibuf[8];
	//	char chbuf[64];

	tmpBuf[0] = RESPONSE_FLAG | CMD_RF_TXDATA;

	debuglog("tx rf data cmd \r\n");

	if (cmd_data_len > 64) {
		cmd_data_len = 64;
	}

	char rfbuf[64] = {0};
	for (int i = 0; i < cmd_data_len && i < 64; i++) {
		rfbuf[i] = cmd_data[i];
	}

	A7105_TxData((uint8_t *)rfbuf, 64);


	// back to rx mode

	A7105_ToRxMode();


	tx_response(tmpBuf, responseFrameLen);
}









