/*
 * main.c
 *
 *  Created on: Jul 13, 2020
 *      Author: Mahmoud Ahmed
 */


/*
 *
 *  3V3         -> bq76PL455A-Q1 EVM J3 pin 3 (VIO)
 *  SCI1_RX     -> bq76PL455A-Q1 EVM J3 pin 5 (TX) **remove R8 from TMS570LS04x Launchpad
 *  SCI1_TX     -> bq76PL455A-Q1 EVM J3 pin 4 (RX)
 *  GND         -> bq76PL455A-Q1 EVM J3 pin 1 (DGND)
 *  GIOA0       -> bq76PL455A-Q1 EVM J3 pin 6 (nWAKE)
 *  GIOA1       -> bq76PL455A-Q1 EVM J3 pin 2 (nFAULT)
 *
 */


#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "system.h"
#include "sys_vim.h"
#include "pl455.h"
#include "Phantom_sci.h"
#include "phantom_can.h"
#include "phantom_pl455.h"
//#include "soc.h"
#include "bms_data.h"
#include "can.h"
#include "esm.h"
#include "sys_core.h"
#include "reg_het.h"
#include "testinterface.h"
#include "pinmux.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "sys_main.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "phantom_freertos.h"
#include "thermistor.h"
#include "temperature_yash.h"
#include "hwConfig.h"

#include "task_balance.h"
#include "task_sensor_read.h"
#include "task_statemachine.h"
#include "task_watchdog.h"

// #include "charger_main.h" TODO: Uncomment when charger code is integrated
// #include "soc.h"


/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void Thermistor_read(void);
void printRandoms(int lower, int upper, int count);

#define CHARGER_ENABLE_PIN  PINMUX_PIN_54_MIBSPI3NCS_5

int UART_RX_RDY = 0;
int RTI_TIMEOUT = 0;

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
extern BMSState_t BMSState;
bms_data BMSDataPtr;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
       phantomSystemInit();

       BMS_init();      // Initialize BMS slaves TODO: Remove this code
       initBMSData(&BMSDataPtr);   // Initializes BMS data structure

       InitializeTemperature();
       setupThermistor();

        if (CHARGER_ENABLE_PIN == 1) { // Pin 17 on X1 connector (MIBSPI3_NCS_5) is used to indicate charging mode
            BMSState = BMS_CHARGING;
        }
        else {
            BMSState = BMS_RUNNING;
        }

       ANDREItaskCreate(); //ANDREI's task initialization stuff to test shit (Comment out)

       xphRtosInit();
       vTaskStartScheduler();

      // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
      while(1);
}




void ANDREItaskCreate(void)
{
    //Watchdog ANDREI
    if (xTaskCreate(vWatchdogTask, (const char*)"WatchdogTask",  240, NULL,  WATCHDOG_TASK_PRIORITY, NULL) != pdTRUE)
    {
        // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
        // probably need a better error handler
        sciSend(PC_UART,23,(unsigned char*)"WatchdogTask Creation Failed.\r\n");
        while(1);
    }

    if ( xTaskCreate(xBlinkTask1, (const char*)"Blink 1 Task", 240, NULL, 0, NULL) != pdTRUE )
    {
        sciSend(PC_UART,23,(unsigned char*)"Blink 1 Task Creation Failed.\r\n");
        while(1);
    }

    if ( xTaskCreate(xBlinkTask2, (const char*)"Blink 2 Task", 240, NULL, 0, NULL) != pdTRUE )
    {
        sciSend(PC_UART,23,(unsigned char*)"Blink 2 Task Creation Failed.\r\n");
        while(1);
    }
}

/* USER CODE BEGIN (4) */

// ANDREIS FREERTOS TEST SHIT ===========================================================================================
void xBlinkTask1(void *pvParameters){

    TickType_t xLastWakeTime1;
    const TickType_t xFreq1 = 333*2;
    xLastWakeTime1 = xTaskGetTickCount();

    while(1){

        vTaskDelayUntil(&xLastWakeTime1, xFreq1);

        UARTSend(scilinREG, "b1\n\r");
        gioSetBit(gioPORTB, 1, !gioGetBit(gioPORTB,1));
        //vTaskDelay(333);
    }
}

void xBlinkTask2(void *pvParameters){

    TickType_t xLastWakeTime2;
    const TickType_t xFreq2 = 499*2;
    xLastWakeTime2 = xTaskGetTickCount();

    while(1){

        vTaskDelayUntil(&xLastWakeTime2, xFreq2);

        UARTSend(scilinREG, "b2\n\r");
        gioSetBit(gioPORTB, 2, !gioGetBit(gioPORTB,2));
        //vTaskDelay(500);
    }
}

// NON TASKS----------------------------------------------------------------------------------------------

// Called periodically every 1ms
void socTimer(TimerHandle_t xTimers)
{
    //socUpdate();
}

/* Timer callback when it expires for the ready to drive sound */
void Timer_2s(TimerHandle_t xTimers)
{

}

/* USER CODE END */

/* USER CODE BEGIN (4) */
void phantomSystemInit()
{
    unsigned char command;

    _enable_IRQ();  //Enables global interrupts
    mibspiInit();   //Initialize the mibspi3 module; mibspi3 = mibspiREG3
    gioInit();      //Initialize the GIO module;
    hetInit();
    sciInit();
    // socInit();

    while ((BMS_UART->FLR & 0x4) == 4);

    _enable_interrupt_();
    canInit();
    canEnableErrorNotification(canREG1);

    sciEnableNotification(PC_UART, SCI_RX_INT);

    sciReceive(PC_UART, 1, (unsigned char *)&command);
    displayPrompt();

    UARTprintf("\n\rBATTERY MANAGEMENT SYSTEM INITIALIZED\n\n\r");
}
/* USER CODE END */


/* USER CODE BEGIN (4) */
/* USER CODE END */
