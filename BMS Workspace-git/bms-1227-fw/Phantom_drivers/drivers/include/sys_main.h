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

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "phantom_freertos.h"

#define CLI_ENABLE true

enum TASK_PRIORITIES{
    STATE_MACHINE_TASK_PRIORITY = 1,
    SENSOR_READ_TASK_PRIORITY = 999 // TODO: need to confirm that this is a sensible priority for sensor read
};

/*********************************************************************************
 *                          DEBUG PRINTING DEFINES
 *********************************************************************************/
#define TASK_PRINT  0
#define STATE_PRINT 0

/*********************************************************************************
 *                          TASK HEADER DECLARATIONS
 *********************************************************************************/
//void vStateMachineTask(void *);  		// This task will evaluate the state machine and decide whether or not to change states
//void vSensorReadTask(void *);    		// This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
//void vSOCTask(void *); 			 		// This task will evaluate the state of charge of the vehicle battery
//void vChargerTask(void *pvParameters);	// This task will call function to charge cells
//void vBalanceTask(void *);		 		// This task will perform passive cell balancing


/*********************************************************************************
 *                          SOFTWARE TIMER INITIALIZATION
 *********************************************************************************/
#define NUMBER_OF_TIMERS   2

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

void socTimer(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);
void phantomSystemInit();


#endif /* INCLUDE_SYS_MAIN_H_ */
