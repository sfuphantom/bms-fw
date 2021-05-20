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

// ANDREI'S GARBAGE - ORGANIZE THESE BETTER (START) /////////////////////////////////////////////////////////
#define WATCHDOG_TASK_PRIORITY         0 //PUT TASKS SOMEWHERE ELSE
#define TASK_PRINT                     1
void ANDREItaskCreate(void);

void vWatchdogTask(void *);             // This task will perform watchdog petting
void xBlinkTask1(void *pvParameters);
void xBlinkTask2(void *pvParameters);

/***************  WATCHDOG GPIO  *********************/
#define WATCHDOG_PORT         hetPORT1
#define WATCHDOG_PIN          2
// ANDREI'S GARBAGE - ORGANIZE THESE BETTER (END) //////////////////////////////////////////////////////////

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


/* USER CODE BEGIN (4) */
/*********************************************************************************
 *                          FreeRTOS Tasks
 *********************************************************************************/
void vStateMachineTask(void *pvParameters){
    uint32 lrval;
    char stbuf[64];
    int nchars;

    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 4000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();
    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        //UARTprintf("battery level = %f\n\r", getBattLevel());

/*********************** STATE MACHINE EVALUATION ***********************************/
        
        if ((BMSState == BMS_RUNNING) || (BMSState == BMS_CHARGING)) { 
            if (STATE_PRINT) {
                if (BMSState == BMS_CHARGING) {
                    UARTSend(PC_UART, "********BMS CHARGING********");
                }
                else { // BMS_RUNNING
                    UARTSend(PC_UART, "********BMS RUNNING********");
                }
                UARTSend(PC_UART, "\n\r");
            }

            // Check all fault flags
            if (BMSDataPtr.Flags.FUSE_FAULT) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "FUSE_FAULT detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.THREE_SECOND_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "THREE_SECOND_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.TOTAL_CELL_ERROR_FLAG)  {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "TOTAL_CELL_ERROR_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.BAD_SLAVE_CONNECTION_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "BAD_SLAVE_CONNECTION_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
        }
    if (BMSState == BMS_FAULT) {
        if (STATE_PRINT) {
            UARTSend(PC_UART, "********BMS FAULT********");
            UARTSend(PC_UART, "\n\r");
        }

        // TODO: Pull down GPIO that is connected to Shutdown Circuit
        // TODO: Send message over CAN so all controllers know we are shutting down
    }


    }while(1);
}


/***********************************************************
 * @function                - vSensorReadTask
 *
 * @brief                   - This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vSensorReadTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 2000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        if(!getBMSinitFlag())
        {
           BMS_init();
        }

        //BMS_Balance_SIM();

        //thermistorRead();

        BMS_Read_All(true);

        //UARTprintf("sensor read task \n\r");
    }while(1);

}

/***********************************************************
 * @function                - vSOCTask
 *
 * @brief                   - This task will update the state of charge and remaining run time estimations, and check for blown fuses)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
/*void vSOCTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        socProcess();

    }while(1);

}*/

/***********************************************************
 * @function                - vChargerTask
 *
 * @brief                   - This task runs functionto charge cells during BMS_CHARGING
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vChargerTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1500; // task frequency in ms - frequency taken to be the same as frequency in sys_main.c

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        // function_to_charge(); TODO: Uncomment when charger code is integrated

    }while(1);

}

/***********************************************************
 * @function                - vBalanceTask
 *
 * @brief                   - This task will passively balance the cells during BMS_CHARGING state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vBalanceTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        BMS_Balance();

    }while(1);

}

/***********************************************************
 * @function                - vWatchdogTask
 *
 * @brief                   - This task will passively balance the cells during BMS_CHARGING state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vWatchdogTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms
    // watchdog timeout is 1.6 seconds

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        //UARTSend(PC_UART, "Pet\r\n");

        if (TASK_PRINT) {UARTSend(PC_UART, "------------->WATCHDOG TASK\r\n");}
        //UARTSend(PC_UART, xTaskGetTickCount());

        gioToggleBit(WATCHDOG_PORT, WATCHDOG_PIN);
    }

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
