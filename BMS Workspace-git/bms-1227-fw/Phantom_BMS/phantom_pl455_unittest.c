/*
 * phantom_pl455_unittest.c
 *
 * Unit tests for BMS slaves.
 * Created on: Feb 21, 2021
 * Author: Amneet Mann
 */

#include <stdio.h>
#include "bms_data.h"
#include "phantom_pl455.h"

extern bms_data* BMSDataPtr;
extern BMS_FLAGS BMS;

/* Testing BMS_Balance function */
void testBMSBalance(void)
{
	// Set-up
	double voltageArray[7] = {3, 4, 3.55, 4.3, 3.2, 4.1, 3.62};
	int count = 0;
	int i;
	BMSDataPtr->Data.minimumCellVoltage = 3.6;
	for (i = 0; i<10; i++)
	{
		BMSDataPtr->SlaveVoltage.BMS_Slave_1[i] = voltageArray[count%7];
		count++;
		BMSDataPtr->SlaveVoltage.BMS_Slave_2[i] = voltageArray[count%7];
		count++;
		BMSDataPtr->SlaveVoltage.BMS_Slave_3[i] = voltageArray[count%7];
		count++;
		BMSDataPtr->SlaveVoltage.BMS_Slave_4[i] = voltageArray[count%7];
		count++;
	}

	// Execute and evaluate result through mock WriteReg() print statements
	// Unit test passed if registers are written with the following values: 588, 403, 612, 153
	BMS_Balance();

	return;
}

int main()
{
	// testBMSBalance();

	return 0;
}