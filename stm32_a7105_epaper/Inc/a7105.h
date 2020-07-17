/*
 * a7105.h
 *
 *  Created on: Jul 1, 2020
 *      Author: chl
 */

#ifndef A7105_H_
#define A7105_H_

#include <stdint.h>

// 2410MHz, note: need to subtract one in receiving mode
extern const uint8_t RF_CHANNEL;


void A7105_Reset(void);

void A7105_ToRxMode(void);
void A7105_TxData(uint8_t * data, int len);

void A7105_WriteReg(uint8_t, uint8_t);
uint8_t A7105_ReadReg(uint8_t);

void ByteSend(uint8_t src);
uint8_t ByteRead(void);

void A7105_WriteID(void);
void A7105_ReadID(void);

void A7105_WriteFIFO(uint8_t *data, int len);

void initRF(void);
void A7105_Config(void);
void A7105_Cal(void);
void A7105_RCO_Cal(void);

uint8_t A7105_CRC_OK(void);

// receive data from fifo
void RxPacket(uint8_t *data, int len);
void StrobeCmd(uint8_t);
void SetCH(uint8_t);


#endif /* A7105_H_ */
