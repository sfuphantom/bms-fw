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

typedef struct bmsSlaveActorMsg_t {
    /* Input Data */
    AgentMessage_t data;
    uint8_t bms_state;

    /* Output Data */
    uint8_t tx_data;

} bmsSlaveActorMsg_t;



#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEACTOR_H_ */
