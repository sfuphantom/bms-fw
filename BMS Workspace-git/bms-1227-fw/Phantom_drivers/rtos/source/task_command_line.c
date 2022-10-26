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

static void HelloWorldTask(void* params){
    while(1){
        UARTprintf("t\r\n");
    }
}


void initializeCommandLine()
{
    xTaskHandle HWT;
        UARTInit(PC_UART, 9600);


    xTaskCreate(HelloWorldTask, "HelloWorldTask", configMINIMAL_STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &HWT);
}


