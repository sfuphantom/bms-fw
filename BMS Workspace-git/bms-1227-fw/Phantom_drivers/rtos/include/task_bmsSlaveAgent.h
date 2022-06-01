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

// Data structure for messages for this agent:
typedef struct bmsSlaveAgentMsg_t {
    /* Input Data */
    AgentMessage_t data;
    uint8_t agent1_state;

    /* Output Data */
    uint8_t tx_data;

} bmsSlaveAgentMsg_t;


#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_BMSSLAVEAGENT_H_ */
