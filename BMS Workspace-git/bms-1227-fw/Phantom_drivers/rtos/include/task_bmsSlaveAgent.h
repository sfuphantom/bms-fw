/*
 * task_bmsSlaveAgent.h
 *
 *  Created on: May 14, 2022
 *      Author: Devon
 */

#ifndef PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEAGENT_H_
#define PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEAGENT_H_

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include <stdlib.h>
#include "queue_common.h"


void vBMSSlaveAgentTask(void *);  // This task will be one of the two-part actor/agent tasks

void bmsSlaveAgentInit(QueueArr_t other); // Initialize the queue


#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEAGENT_H_ */
