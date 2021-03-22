/*
 * soc_unittest.c
 *
 * Unit tests for SOC module. 
 * Created on: Jan 5, 2021
 * Author: Amneet Mann
 */

#include <stdlib.h>
#include <stdio.h>
#include "soc.h"

// Define test functions up here (we can have a separate file for test functions later if needed)

const float eps = 0.005;
const int eps_large = 10;

extern SOCState_t SOCtate;
extern float battLevel; 
extern float battCapacity;
extern bms_data* BMSDataPtr;
extern float lastCurrentValue;

/* Testing getSOCFromVoltage function */
void testGetSOCFromVoltage(void)
{
	printf("Testing getSOCFromVoltage():\n");

	float test_val;

	SOCState = CHARGE;
	test_val = 	getSOCFromVoltage(4, 5, 100);
	if (abs(test_val - 87.5) < eps)
	{
		printf("PASS: With an initial estimation of 100%%, SOC for battery at 4V and 5 degrees predicted to be %f%%\n", test_val);
	}
	else
	{
		printf("FAIL: With initial estimation of 100%%, SOC for battery at 4V and 5 degrees predicted to be %f%%\n", test_val);
	}
	test_val = 	getSOCFromVoltage(4, 5, 0);
	if (abs(test_val - 87.5) < eps)
	{
		printf("PASS: With an initial estimation of 0%%, SOC for battery at 4V and 5 degrees predicted to be %f%%\n", test_val);
	}
	else
	{
		printf("FAIL: With initial estimation of 0%%, SOC for battery at 4V and 5 degrees predicted to be %f%%\n", test_val);
	}

	printf("\n");
}

/* Testing GetOCV function */
void testGetOCV(void)
{
	printf("Testing getOCV():\n");

	float test_val;

	SOCState = CHARGE;
	test_val = getOCV(62, 5);
	if (abs(test_val - 3.7795) < eps)
	{
		printf("PASS: OCV for battery charging with 62%% SOC at 5 degrees predicted to be %fV\n", test_val);
	}
	else
	{
		printf("FAIL: OCV for battery charging with 62%% SOC at 5 degrees predicted to be %fV\n", test_val);
	}

	test_val = getOCV(87, 32);
	if (abs(test_val - 3.9929) < eps)
	{
		printf("PASS: OCV for battery charging with 87%% SOC at 32 degrees predicted to be %fV\n", test_val);
	}
	else
	{
		printf("FAIL: OCV for battery charging with 87%% SOC at 32 degrees predicted to be %fV\n", test_val);
	}

	SOCState = DISCHARGE;
	test_val = getOCV(2, 77);
	if (abs(test_val - 3.2558) < eps)
	{
		printf("PASS: OCV for battery discharging with 2%% SOC at 77 degrees predicted to be %fV\n", test_val);
	}
	else
	{
		printf("FAIL: OCV for battery discharging with 2%% SOC at 77 degrees predicted to be %fV\n", test_val);
	}

	printf("\n");
}

/* Testing updateSOC function */
void testUpdateSOC(void)
{
	printf("Testing updateSOC():\n");

	SOCState = DISCHARGE;
	battLevel = 5000;
	battCapacity = 6840;

	BMSDataPtr = malloc(sizeof(bms_data));
	initBMSData(BMSDataPtr);
	updateSOC(10, 0.2);
	float test_val = getSOC();
	if (abs(test_val - 73.0993) < eps)
	{
		printf("PASS: SOC successfully updated after 10ms to %f%%\n", test_val);
	}
	else
	{
		printf("FAIL: SOC updated to %f%%\n", test_val);
	}

	printf("\n");
}

/* Testing updateBattCapacity function */
/* Current test valid for when:
	getInstantaneousVoltage() always returns 3.9
	getInstantaneousCurrent() always returns 1 
	xTaskGetTickCount() always returns 5,000,000 */
void testUpdateBattCapacity(void)
{
	printf("Testing updateBattCapacity():\n");

	SOCState = LOW_DISCHARGE;
	battCapacity = 6840;
	battLevel = 5000;
	lastCurrentValue = 1;

	BMSDataPtr = malloc(sizeof(bms_data));
	initBMSData(BMSDataPtr);
	BMSDataPtr->Data.SOC = 73.2;
	updateBattCapacity();
	float test_val = getBattCapacity();
	if (abs(test_val - 6840) < eps_large)
	{
		printf("PASS: With constant current and voltage, battery capacity remains constant at %fC\n", test_val);
	}
	else
	{
		printf("FAIL: Battery capacity updated to %f\n", test_val);
	}

	printf("\n");
}

// Run test functions in here
int main(void)
{
	// testGetSOCFromVoltage();
	// testGetOCV();
	// testUpdateSOC();
	// testUpdateBattCapacity();
	return 0;
}
