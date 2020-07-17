#ifndef UARTTASK_H
#define UARTTASK_H

#include <stdint.h>

void parse_rx_buf(const char *buf, int len);

void report_rf_data(const uint8_t * buf, int len);

#endif
