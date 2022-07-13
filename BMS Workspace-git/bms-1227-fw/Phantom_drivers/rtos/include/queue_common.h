/*
 * queue_common.h
 * Typedefs for structures used in agent, actor-type software architecture
 *
 *  Created on: May 13, 2022
 *      Author: dlutz
 */

#ifndef PHANTOM_DRIVERS_RTOS_INCLUDE_QUEUE_COMMON_H_
#define PHANTOM_DRIVERS_RTOS_INCLUDE_QUEUE_COMMON_H_

#include "stdio.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"

/* Agent Message Structure */
typedef struct AgentMessage {
    uint8_t id;  // agent identifier
    uint8_t msg; // data from agent
} AgentMessage_t;

// Data structure for messages for this agent:
typedef struct bmsSlaveMsg_t {
    /* Input Data */
    AgentMessage_t data;
    uint8_t agent_state;
    uint8_t actor_state;

    /* Output Data */
    uint8_t tx_data;

} bmsSlaveMsg_t;

typedef struct QueueArr_t {
    QueueHandle_t tx;
    QueueHandle_t rx;
} QueueArr_t;


#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_QUEUE_COMMON_H_ */
