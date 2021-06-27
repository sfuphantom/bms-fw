/*
 * charger.h
 *
 *  Created on: Feb 17, 2021
 *      Author: elena
 */

#ifndef PHANTOM_DRIVERS_SOURCE_CHARGER_H_
#define PHANTOM_DRIVERS_SOURCE_CHARGER_H_

//#include <stdio.h>
//#include "sys_main.h"
//#include "phantom_can.h"

int maxVoltage;
int maxCurrent;
int minCurrent;
int outputCurrent;
uint16_t chargerData[];

void function_to_charge();
void sendChargerMsg1();
//void write_data_to_file();

#endif /* PHANTOM_DRIVERS_SOURCE_CHARGER_H_ */
