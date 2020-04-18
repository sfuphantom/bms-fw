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


typedef enum {CHARGING, RUNNING, FAULT} State;

enum TASK_PRIORITIES{
    STATE_MACHINE_TASK_PRIORITY = 1,
    SENSOR_READ_TASK_PRIORITY
};

typedef struct CAN_MSG{

} CAN_MSG;

#endif /* INCLUDE_SYS_MAIN_H_ */
