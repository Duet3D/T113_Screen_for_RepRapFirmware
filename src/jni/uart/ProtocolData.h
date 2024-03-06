/*
 * ProtocolData.h
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */

#ifndef _UART_PROTOCOL_DATA_H_
#define _UART_PROTOCOL_DATA_H_

#include <string>

typedef struct {
	const unsigned char* data;
	unsigned int len;
} SProtocolData;

#endif /* _UART_PROTOCOL_DATA_H_ */
