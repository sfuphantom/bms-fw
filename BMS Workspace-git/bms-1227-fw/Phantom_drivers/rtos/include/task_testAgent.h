/*
 * task_testAgent.h
 *
 *  Created on: April 3, 2022
 *      Author: dlutz
 */

#ifndef PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTAGENT_H_
#define PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTAGENT_H_

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include <stdlib.h>
#include "queue_common.h"


void vTestAgentTask(void *);  // This task will be one of the two-part actor/agent tasks

void testAgentInit(QueueArr_t other); // Initialize the queue

// Data structure for messages for this agent:
typedef struct testAgentMsg_t {
    /* Input Data */
    AgentMessage_t data;
    uint8_t agent1_state;

    /* Output Data */
    uint8_t tx_data;

} testAgentMsg_t;


#endif /* PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTAGENT_H_ */
