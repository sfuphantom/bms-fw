/*
 * sys_main.h
 *
 *  Created on: Feb 17, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_SYS_MAIN_H_
#define INCLUDE_SYS_MAIN_H_

#include "datatypes.h"
#include "pl455.h"

#define BMSByteArraySize  43


typedef enum {CHARGING, RUNNING, FAULT} State;

enum TASK_PRIORITIES{
    STATE_MACHINE_TASK_PRIORITY = 1,
    SENSOR_READ_TASK_PRIORITY
};

void my_sciInit(void);
void my_rtiInit(void);
void BMS_init(void);
void BMS_Read_Single(uint8_t device);
void BMS_Read_Single_NP(uint8_t device);
void BMS_Read_All(void);
void BMS_Read_All_NP();
void setBMSTimerPeriod(uint32 timems);
void Thermistor_Read(void);
void BMS_Slaves_Heartbeat(void);
void BMS_Balance();
int GetTimeout(void);
void getCurrentReadings(void);
void getBMSSlaveArray(BYTE BMSArray[BMSByteArraySize*(TOTALBOARDS)]);

typedef struct CAN_MSG{

} CAN_MSG;

#endif /* INCLUDE_SYS_MAIN_H_ */
