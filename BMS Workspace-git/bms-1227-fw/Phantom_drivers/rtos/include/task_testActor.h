/*
 * task_testActor.h
 * Template-style header for what could be used as a starting point for an actor in the agent-actor architecture.
 *
 *  Created on: April 3, 2022
 *      Author: dlutz
 */

#ifndef PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTACTOR_H_
#define PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTACTOR_H_
#include "queue_common.h"

void vTestActorTask(void *);                  // This task will be one of the two-part actor/agent tasks

// This struct could be modified based on the needs of the actor's business logic.
// Currently passes around a uint8_t
typedef struct testActorMsg_t {
    /* Input Data */
    AgentMessage_t data;
    uint8_t agent1_state;

    /* Output Data */
    uint8_t tx_data;

} testActorMsg_t;

#endif /* PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_TESTACTOR_H_ */
