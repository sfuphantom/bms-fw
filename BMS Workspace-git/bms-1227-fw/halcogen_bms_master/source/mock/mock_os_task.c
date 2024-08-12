#include <stdio.h>
#include <stdlib.h>
#include "hal_stdtypes.h"
#include "os_portmacro.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_task.h"

#define vTaskDelayUntil							MPU_vTaskDelayUntil		
#define xTaskGetTickCount						MPU_xTaskGetTickCount			

void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement )
{
	*pxPreviousWakeTime = *pxPreviousWakeTime + 100000;
	return;
}

TickType_t xTaskGetTickCount( void )
{
	TickType_t xTicks = 5000000;
	return xTicks;
}
