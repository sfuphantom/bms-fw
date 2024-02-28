/*
 *  task_imd.h
 *
 *  Created on: June 22, 2023
 *  Author: Karina Kramer
 *
 *  FreeRTOS version of IMD.h created by Sumreen Rattan on May 21, 2020
 */
#ifndef SOURCE_IMD_H_
#define SOURCE_IMD_H_

/* Include Files */
#include "het.h"
#include "gio.h"
#include "sys_vim.h"
#include "rti.h"
#include "sci.h"
#include "stdlib.h" // contains ltoa
#include "sys_common.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
//#include "timers.h"
#include "os_task.h"
#include "IMD.h"

/* Function: vMonitorIMD()
 *
 * Purpose: Sends a signal from BMS Master to HV Shutdown if there is an IMD failure.
 * IMD failure refers to the IMD switching to a state that is outside of the normal
 * range. Refer to "IMD Testing Update" for more info.
 *
 */
void vMonitorIMD();

#endif /* SOURCE_IMD_H_ */
