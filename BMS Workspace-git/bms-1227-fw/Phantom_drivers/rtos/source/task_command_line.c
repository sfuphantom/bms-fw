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

static uint8 previous;
static xTaskHandle commandLineTaskHandle;
static void CommandLineTask(void* params){
    while(1){
        uint8 msg;
        sciReceive(PC_UART, 1, &msg);
        if(msg==previous){
            // do nothing
        } else if(msg=='1'){
            UARTprintf("Message 1 received\r\n");
        } else if(msg=='2'){
            UARTprintf("Message 2 received\r\n");
        } else if(msg=='3'){
            UARTprintf("Message 3 received\r\n");
        }
        previous = msg;
        vTaskDelay(1000);
    }
}

void initializeCommandLine()
{
        UARTInit(PC_UART, 9600);

    xTaskCreate(CommandLineTask, "CommandLineTask", configMINIMAL_STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &commandLineTaskHandle);
}


