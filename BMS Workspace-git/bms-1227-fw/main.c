/*
 * main.c
 *
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
#include "stdio.h"
#include "system.h"
#include "etpwm.h"
#include "ecap.h"
#include "hal_stdtypes.h"

#include "task_imd.h"
#include <task_slave_pipeline.h>
#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "Phantom_sci.h"
#include "can.h"
#include "reg_het.h"
#include "sys_main.h"
#include "sys_core.h"
#include "soc.h"
#include "phantom_pl455.h"
#include "pinmux.h"
#include "testinterface.h"
#include "agentactor.h"
#include "mibspi.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "phantom_freertos.h"

#include "hwConfig.h"

//#include "IMD.h"
#include "sys_common.h"
#include "reg_ecap.h" //for ecapREG1 in capGetSignal call
#include "ecap.h"

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


int UART_RX_RDY = 0;
int RTI_TIMEOUT = 0;

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/
extern BMSState_t BMSState;
/* USER CODE END */

float64 globle_period,globle_duty;
const ecapBASE_t* ecapREG1G = 0xFCF79300U;

int main(void)
{
    /* USER CODE BEGIN (3) */
//    do{

    initBMSData(); // Initializes BMS data structure and ensures pointers are set properly
    // phantomSystemInit();
    _enable_interrupt_();
    /* Alternate code for configuring ETPWM and ECAP */
        /* Configure ETPWM1 */
    /* Set the TBCLK frequency =  VCLK4 frequency = 90MHz */
    etpwmSetClkDiv(etpwmREG1, ClkDiv_by_1, HspClkDiv_by_1);

    /* Set the time period as 1000 ns (Divider value = (1000ns * 90MHz) - 1 = 89)*/
    etpwmSetTimebasePeriod(etpwmREG1, 89);

    /* Configure Compare A value as half the time period */
    etpwmSetCmpA(etpwmREG1, 45);

    /* Configure mthe module to set PWMA value as 1 when CTR=0 and as 0 when CTR=CmpA  */
    etpwmActionQualConfig_t configPWMA;
    configPWMA.CtrEqZero_Action = ActionQual_Set;
    configPWMA.CtrEqCmpAUp_Action = ActionQual_Clear;
    configPWMA.CtrEqPeriod_Action = ActionQual_Disabled;
    configPWMA.CtrEqCmpADown_Action = ActionQual_Disabled;
    configPWMA.CtrEqCmpBUp_Action = ActionQual_Disabled;
    configPWMA.CtrEqCmpBDown_Action = ActionQual_Disabled;
    etpwmSetActionQualPwmA(etpwmREG1, configPWMA);

    /* Start counter in CountUp mode */
    etpwmSetCount(etpwmREG1, 0);
    etpwmSetCounterMode(etpwmREG1, CounterMode_Up);
    etpwmStartTBCLK();

    /* Configure ECAP1 */
    /* Configure Event 1 to Capture the rising edge */
    ecapSetCaptureEvent1(ecapREG1, RISING_EDGE, RESET_DISABLE);

    /* Configure Event 2 to Capture the falling edge */
    ecapSetCaptureEvent2(ecapREG1, FALLING_EDGE, RESET_DISABLE);

    /* Configure Event 3 to Capture the rising edge with reset counter enable */
    ecapSetCaptureEvent3(ecapREG1, RISING_EDGE, RESET_ENABLE);

    /* Set Capure mode as Continuous and Wrap event as CAP3  */
    ecapSetCaptureMode(ecapREG1, CONTINUOUS, CAPTURE_EVENT3);

    /* Start counter */
    ecapStartCounter(ecapREG1);

    /* Enable Loading on Capture */
    ecapEnableCapture(ecapREG1);

    /* Enable Interrupt for CAP3 event */
    ecapEnableInterrupt(ecapREG1, ecapInt_CEVT3);
    
// #endif
    /*  ... run forever  */
    while(1);

    // initalizeIMD();     //TODO: list all initializations here before entering state machine

    // Register the BMS agent and actor tasks:
//    if(initSlavePipeline())
//    {
//        while(true){
//            // TODO: spam printing debug messages
//        }
//    }
//    // BMS_init();      // Initialize BMS slaves. Initialization must be re-added after PL455 rewrite.
//
//    // TODO: Initialize modern temperature here. Replaces line: InitializeTemperature() and setupThermistor()
//
//    if (true)
//    { // Pin 17 on X1 connector (MIBSPI3_NCS_5) is used to indicate charging mode
//        BMSState = BMS_CHARGING;
//    }
//    else
//    {
//        BMSState = BMS_RUNNING;
//     }

    //etpwmInit();
    ecapInit();

    hetSIGNAL_t * capturedSignal;


    // Log captured signal for debugging
    //UARTprintf("Captured Signal: Period = %u, Duty = %u\n", capturedSignal.period, capturedSignal.duty);
    // Read and process PWM values
//    readPWMValues(hetRAM1, pwmChannel);
    //}

    xphRtosInit();
    vTaskStartScheduler();

    // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
//}
    while (1);

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

