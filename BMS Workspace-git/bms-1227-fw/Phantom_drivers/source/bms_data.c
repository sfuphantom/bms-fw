/**
 * bms_data.c
 * Author: Amneet Mann
 * Date: February 8, 2021
 */

#include "bms_data.h"

void initBMSData(bms_data* BMSDataPtr)
{
	/***********************************************************
     *              FLAG INITIALIZATION
     ***********************************************************/
	BMSDataPtr->Flags.BALANCE_EN = 0;
	BMSDataPtr->Flags.FUSE_FAULT = 0;

	/***********************************************************
     *              SOC INITIALIZATION
     ***********************************************************/
	BMSDataPtr->SOC.SOC = 0;
	BMSDataPtr->SOC.remainingRunTime = 0;
}
