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

#include <task_slave_pipeline.h>
#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "Phantom_sci.h"
#include "can.h"
#include "reg_het.h"
#include "sys_main.h"
#include "soc.h"
#include "phantom_pl455.h"
#include "pl455.h"
#include "pinmux.h"
#include "testinterface.h"
#include "agentactor.h"
#include "hv_driver.h"
#include "mibspi.h"
#include "het.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "phantom_freertos.h"
#include "hwConfig.h"
#include "a_tp_sci.h"


#include "sys_common.h"

// Includes for HV Voltage reading driver test

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
void printRandoms(int lower, int upper, int count);


int UART_RX_RDY = 0;
int RTI_TIMEOUT = 0;
int readcheck = 0;
/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
BMSState_t BMSState;
/* USER CODE END */

int main(void)
    {
    /* USER CODE BEGIN (3) */
/* CAN TEST
    canInit();

    uint8_t tx_data[9] = { 'P', 'H', 'A', 'N', 'T', 'O', 'M', 'm', "\0" };
    while(1) {
        canTransmit(canREG1, canMESSAGE_BOX1, tx_data);
        delayms(500);
    }
*/
   /*
    uint8_t rx_data[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    while(!canIsRXMessageArrived(canREG1, canMESSAGE_BOX1)));
    canGetData(canREG1, canMESSAGE_BOX1, rx_data);
    */


    UARTprintf("begin main");
    initBMSData(); // Initializes BMS data structure and ensures pointers are set properly
    phantomSystemInit();

    // Register the BMS agent and actor tasks:
    if(!initSlavePipeline())
    {
        while(true){
            // TODO: spam printing debug messages
            UARTprintf("Unable to initialize slave pipeline!\r\n");
        }
    }
    // BMS_init();      // Initialize BMS slaves. Initialization must be re-added after PL455 rewrite.

    // TODO: Initialize modern temperature here. Replaces line: InitializeTemperature() and setupThermistor()

    if (true)
    { // Pin 17 on X1 connector (MIBSPI3_NCS_5) is used to indicate charging mode
        BMSState = BMS_CHARGING;
    }
    else
    {
        BMSState = BMS_RUNNING;
    }



//    while(1)
//    {
//        simulateVoltageHVADC(2021); // this function uses mibspi3 to simulate the behavior of the ADS7044 ADC that reads HV voltage
//        getBatteryVoltageHV();         // this reads voltage from the ADS7044, it drives the mibspi1 CS[0] pin low which initiates the transfer
//
//    }


    _enable_IRQ();  // Enables global interrupts
    mibspiInit();   // Initialize the mibspi3 module; mibspi3 = mibspiREG3
    gioInit();

    // BMS slave testing loop
    //tp_sciInit(); // custom SCI init function

    sciInit();
    hetInit();
    //sciSetBaudrate(BMS_UART, BAUDRATE);
    BMS_init();
    //CommClear();
    //CommReset();
    //WakePL455();


    while(1) {
        //BMS_Balance();
        BMS_Read_All(1);
        //BMS_Read_Single(0);
        //BMS_Read_Single(1);
        //BMS_ProcessState();
        //getCurrentReadings();
        //CommClear();
        //CommReset();
        if(BMSDataPtr->SlaveVoltage.BMS_Slave_1[0] != 0) readcheck ++;
        if(BMSDataPtr->SlaveVoltage.BMS_Slave_1[2] != 0) readcheck ++;
        if(BMSDataPtr->SlaveVoltage.BMS_Slave_1[4] != 0) readcheck ++;
        delayms(500);
    }


    xphRtosInit();  // initializes all FreeRTOS tasks and timers

    // start FreeRTOS task scheduling
    vTaskStartScheduler();

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);

}



/* USER CODE BEGIN (4) */

// Called periodically every 1ms
void socTimer(TimerHandle_t xTimers)
{
    // socUpdate(); // TODO investigate whether this timer (used in phantom_freertos.c:38) is still required for something.
    UARTprintf("\n\rBREAKPOINT TEST LINE\n\n\r");
}

/* Timer callback when it expires for the ready to drive sound */
void Timer_2s(TimerHandle_t xTimers)
{
    // TODO: investigate whether this timer (used in phantom_freertos.c:56) is still required for something.
}

/* USER CODE BEGIN (4) */
void phantomSystemInit()
{
    unsigned char command;

    _enable_IRQ();  // Enables global interrupts
    mibspiInit();   // Initialize the mibspi3 module; mibspi3 = mibspiREG3
    gioInit();      // Initialize the GIO module;
    hetInit();
    sciInit();
    socInit();

    while ((BMS_UART->FLR & 0x4) == 4);

    //enable_interrupt_();
    canInit();
    canEnableErrorNotification(canREG1);

    UARTInit(PC_UART, 9600);

    sciEnableNotification(PC_UART, SCI_RX_INT);

    sciReceive(PC_UART, 1, (unsigned char *)&command);
    displayPrompt();
    UARTprintf("\n\rBATTERY MANAGEMENT SYSTEM INITIALIZED\n\n\r");
}
/* USER CODE END */


/* USER CODE BEGIN (4) */
/* USER CODE END */
