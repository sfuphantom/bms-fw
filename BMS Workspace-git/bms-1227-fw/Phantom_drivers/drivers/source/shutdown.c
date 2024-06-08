/*
 * shutdown.c
 *
 *  Created on: Jun 1, 2024
 *      Author: hihid
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


#include "sys_common.h"
#include "shutdown.h"



int shutdown(void){
    // Initialize the GIO module
    gioInit();
    BMSState_t BMSState;
    BMSState = BMS_FAULT;

    // Configure GIOA5 as an output pin
    gioSetDirection(gioPORTA, gioPORTA->DIR | (1 << 5));

    while (1)
    {
        int i;
        gioSetBit(gioPORTA, 5, 1);
        for(i=0; i<99999999; i++){int a = i;}//delay
        gioSetBit(gioPORTA, 5, 0);
        for(i=0; i<99999999; i++){int a = i;}///delay
        //checkFaultAndShutdown(BMSState);
    }

    return 0;
}

void checkFaultAndShutdown(BMSState_t BMSState){
    if (BMSState == BMS_FAULT){
        // Set GIOA5 high
        gioSetBit(gioPORTA, 5, 1);

        //Set GIOA5 low when reset (TBD)
        //if(reset){
        //gioSetBit(gioPORTA, 5, 0);}

    }
    else
    {
       return;
    }
}


