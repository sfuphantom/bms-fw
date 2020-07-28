/*
 * soc.h
 *
 *  Created on: Jul 20, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef PHANTOM_DRIVERS_INCLUDE_SOC_H_
#define PHANTOM_DRIVERS_INCLUDE_SOC_H_

#include "hal_stdtypes.h"

#define SOC_TIMER_PERIOD 0.05

void socInit(void);
void socUpdate(void);
float getBattLevel(void);


#endif /* PHANTOM_DRIVERS_INCLUDE_SOC_H_ */
