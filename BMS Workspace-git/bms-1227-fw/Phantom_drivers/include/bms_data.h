/**
 * bms_data.h
 * Author: Amneet Mann
 * Date: February 8, 2021
 */

#ifndef BMS_DATA_H
#define BMS_DATA_H

#include "sys_common.h"

typedef struct flags
{
	uint8_t BALANCE_EN; // If 1, then cell balancing is enabled (only done during charging), if 0, then cell balancing is disabled
						// TODO: Set this = 1 if state machine evaluates to CHARGING state
	uint8_t FUSE_FAULT; // If 1, then fuse has blown, if 0, then fuses are intact
} flags;

typedef struct SOCInfo
{
	float SOC; // State of charge estimation, expressed in % (i.e, 55.5% = 55.5)
	float remainingRunTime; // Remaining run time estimation, expressing in minutes
} SOCInfo;

typedef struct bms_data
{
    flags Flags;
    SOCInfo SOC;
} bms_data;

void initBMSData(bms_data* BMSDataPtr);

#endif // BMS_DATA_H