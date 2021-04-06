/**
 * bms_data.c
 * Author: Amneet Mann
 * Date: February 8, 2021
 */

#include <string.h>
#include "bms_data.h"

void initBMSData()
{
	/***********************************************************
     *              FLAG INITIALIZATION
     ***********************************************************/
	BMSDataPtr->Flags.BALANCE_EN = 0;
	BMSDataPtr->Flags.FUSE_FAULT = 0;
	BMSDataPtr->Flags.THREE_SECOND_FLAG = 0;
	BMSDataPtr->Flags.BAD_SLAVE_CONNECTION_FLAG = 0;

	/***********************************************************
     *              DATA INITIALIZATION
     ***********************************************************/
	BMSDataPtr->Data.SOC = 0;
	BMSDataPtr->Data.remainingRunTime = 0;
	BMSDataPtr->Data.minimumCellVoltage = 5;
	BMSDataPtr->Data.state = BMS_RUNNING;

	/***********************************************************
     *              SLAVE VOLTAGE INITIALIZATION
     ***********************************************************/
	memset(BMSDataPtr->SlaveVoltage.BMS_Slave_1, 0, CELLS_PER_BOARD*sizeof (double));
	memset(BMSDataPtr->SlaveVoltage.BMS_Slave_2, 0, CELLS_PER_BOARD*sizeof (double));
	memset(BMSDataPtr->SlaveVoltage.BMS_Slave_3, 0, CELLS_PER_BOARD*sizeof (double));
	memset(BMSDataPtr->SlaveVoltage.BMS_Slave_4, 0, CELLS_PER_BOARD*sizeof (double));
}
