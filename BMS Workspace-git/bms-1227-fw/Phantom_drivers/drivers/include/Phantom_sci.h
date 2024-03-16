/*
 * Phantom_sci.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_PHANTOM_SCI_H_
#define INCLUDE_PHANTOM_SCI_H_

#include "sci.h"

#ifdef __cplusplus
extern "C" {
#endif

void UARTInit(sciBASE_t *sci, uint32 baud);
void UARTSend(sciBASE_t *sci, char data[]);

// hv_driver.c/h has a competing definition of UARTprintf
//void UARTprintf(const char *_format, ...);

#endif /* INCLUDE_PHANTOM_SCI_H_ */
