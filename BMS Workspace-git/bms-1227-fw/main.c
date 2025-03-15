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
#include "pinmux.h"
#include "testinterface.h"
#include "agentactor.h"
#include "hv_driver.h"
#include "mibspi.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "phantom_freertos.h"
#include "hwConfig.h"

#include "shutdown.h"
#include "sys_common.h"

// Includes for HV Voltage reading driver test

/* USER CODE BEGIN (1) */
#include "can.h"
#include "system.h"
#include "sci.h"
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

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
BMSState_t BMSState;

#define D_SIZE 9

uint8 tx_data[D_SIZE] = {'H', 'E', 'R', 'C', 'U', 'L', 'E', 'S', '\0'};
uint32 checkPackets(uint8 *src_packet, uint8 *dst_packet, uint32 psize);

uint8 rx_data[D_SIZE] = {0};
uint32 error = 0;
/* USER CODE END */

int main(void)
  {
    /* USER CODE BEGIN (3) */
    /*
    UARTprintf("begin main");
    initBMSData(); // Initializes BMS data structure and ensures pointers are set properly
    phantomSystemInit();

    // Register the BMS agent and actor tasks:
    //if(!initSlavePipeline())
    if(false)
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




    // initializes all FreeRTOS tasks and timers
    xphRtosInit();

    // start FreeRTOS task scheduling
    vTaskStartScheduler();

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
    while(1);
    */

    //Initialize can 1 for transmitting and can2 for receiving
        canInit();

        //Initialize sci for receiving
        sciInit();

        //transmit on can1
        canTransmit(canREG1, CANMESSAGE_BOX1, tx_data);

        while(!canIsRxMessageArrived(canREG2, CANMESSAGE_BOX1));
        canGetData(canREG2, CANMESSAGE_BOX1, rx_data);

        sciSend(scilinREG, D_SIZE, rx_data);

        error = checkPackets(&tx_data[0], &rx_data[0], D_SIZE);


        //run forever
        while(1);
        return 0;
}



/* USER CODE BEGIN (4) */

uint32 checkPackets(uint8 *src_packet, uint8 *dst_packet, uint32 psize){
    uint32 err = 0;
    uint32 cnt = psize;

    while(cnt--){
        if((*src_packet++) != (*dst_packet++)){
            err++;
        }
    }
    return err;
}

void canMessageNotification(canBASE_t *node, uint32 messageBox ){
    return;
}

void canErrorNotification(canBASE_t *node, uint32 notification){
    return;
}

void esmGroup1Notification(unsigned channel){
    return;
}

void esmGroup2Notification(unsigned channel){
    return;
}


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
