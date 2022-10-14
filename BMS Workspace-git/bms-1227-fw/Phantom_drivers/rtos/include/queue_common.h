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


#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_QUEUE_COMMON_H_ */
