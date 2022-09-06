/*
 * task_bmsSlaveActor.h
 *
 *  Created on: May 14, 2022
 *      Author: dlutz
 */

#ifndef PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEACTOR_H_
#define PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEACTOR_H_
#include "queue_common.h"

void vBMSSlaveActorTask(void *);                  // This task will be one of the two-part actor/agent tasks

void bmsSlaveActorInit(QueueArr_t other);



#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEACTOR_H_ */
