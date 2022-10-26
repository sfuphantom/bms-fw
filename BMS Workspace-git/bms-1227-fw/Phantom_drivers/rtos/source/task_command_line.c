/*
 * task_command_line.c
 *
 *  Created on: Oct 25, 2022
 *      Author: lonam
 */
#include "task_command_line.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

#include "Phantom_sci.h"
#include "hwConfig.h"

static xTaskHandle commandLineTaskHandle;
static void CommandLineTask(void* params){
    while(1){
        UARTprintf("Hello World!\r\n");
        vTaskDelay(1000);

    }
}


void initializeCommandLine()
{
        UARTInit(PC_UART, 9600);


    xTaskCreate(CommandLineTask, "CommandLineTask", configMINIMAL_STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &commandLineTaskHandle);
}


