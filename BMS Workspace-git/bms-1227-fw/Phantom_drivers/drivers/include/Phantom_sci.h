/*
 * Phantom_sc.h
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
//void UARTprintf(const char *_format, ...); hv_driver has its own UARTprintf with a different definition - using that one for hv current driver test

#endif /* INCLUDE_PHANTOM_SCI_H_ */
