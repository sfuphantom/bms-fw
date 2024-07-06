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



void shutdownHV(void){
    // Initialize the GIO module
    //gioInit();

    // Configure GIOA5 as an output pin
    // bit masking to set direction of gioPORTA to the direction of the 5th bit (BMS_FLT_PIN).
    gioSetDirection(gioPORTA, gioPORTA->DIR | (1 << 5));
    gioSetBit(gioPORTA, BMS_FLT_PIN, PIN_LEVEL_ON);
    BMSState = BMS_FAULT;

    //TODO: reset conditions?

    return;
}



