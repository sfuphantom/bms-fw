/*
 * task_slave_pipeline.h
 *
 *  Created on: Oct 10, 2022
 *      Author: rafgu
 */

#ifndef PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_SLAVE_PIPELINE_H_
#define PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_SLAVE_PIPELINE_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

#include "queue_common.h"

#include "stdint.h"

typedef struct SlavePipeline_t
{
	TaskHandle_t  consumer;

	TaskHandle_t  producer;

	QueueHandle_t queue;

} SlavePipeline_t;

typedef struct bmsSlaveMsg{

	uint8_t data;

} bmsSlaveMsg_t;


// initialize all threads and queues
uint8_t initSlavePipeline();



#endif /* PHANTOM_DRIVERS_RTOS_INCLUDE_TASK_SLAVE_PIPELINE_H_ */
