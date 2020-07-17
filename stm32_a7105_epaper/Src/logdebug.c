/*
 * logdebug.c
 *
 *  Created on: Jun 28, 2020
 *      Author: chl
 */

#include "logdebug.h"

#include "stm32f1xx_hal.h"

#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;


#define LOG_BUF_SIZE 128

void debuglog(const char * format, ...)
{

	char buffer[LOG_BUF_SIZE];
	va_list args;
	va_start (args, format);
	int logLen =  vsnprintf (buffer,LOG_BUF_SIZE,format, args);
	va_end (args);

	if (logLen > 0 && logLen <= LOG_BUF_SIZE) {
		HAL_UART_Transmit(&huart2, (uint8_t *)buffer, logLen, 1000);
	}
}
