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
#include "can.h"
#include "esm.h"
#include "sys_core.h"
#include "reg_het.h"
#include "testinterface.h"

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
#include "soc.h"


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


int UART_RX_RDY = 0;
int RTI_TIMEOUT = 0;

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
State state = RUNNING;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */


       phantomSystemInit();

       BMS_init();

       // CAN Testing
//       CANSend();
//       sendVoltageArray();


       UARTprintf("start");
       // CAN Charger Testing


       while(1){
           testMessage1();
           delayms(1500);
       }


//       sendVoltageArray();
       //sendVoltageBox23(data[])
       //readvaluefromqueue

       while(1);

       InitializeTemperature();
       setupThermistor();

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

        BMS_Read_All_NP();

        //UARTprintf("sensor read task \n\r");
    }while(1);

}

// Called periodically every 1ms
void socTimer(TimerHandle_t xTimers)
{
    //socUpdate();
}

/* Timer callback when it expires for the ready to drive sound */
void Timer_2s(TimerHandle_t xTimers)
{

}

/* USER CODE BEGIN (4) */
void phantomSystemInit()
{
    unsigned char command;

    _enable_IRQ();  //Enables global interrupts
    mibspiInit();   //Initialize the mibspi3 module; mibspi3 = mibspiREG3
    gioInit();      //Initialize the GIO module;
    hetInit();
    sciInit();
    socInit();

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
