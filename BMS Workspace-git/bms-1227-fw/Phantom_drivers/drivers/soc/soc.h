/*
 * soc.h
 *
 *  Created on: Jul 20, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef PHANTOM_DRIVERS_INCLUDE_SOC_H_
#define PHANTOM_DRIVERS_INCLUDE_SOC_H_

#include <math.h>
#include "hal_stdtypes.h"
#include "os_portmacro.h"
#include "bms_data.h"

typedef enum
{
	INIT,
	CHARGE,
	DISCHARGE,
	LOW_DISCHARGE
} SOCState_t;

/* Parameters of OCV model */
// Charging at 5 degrees
static const float p0_Charge5 = 3.734;
static const float p1_Charge5 = -0.2765;
static const float p2_Charge5 = 8.013*0.00001;
static const float alpha1_Charge5 = -0.001155;
static const float alpha2_Charge5 = -0.06674;
// Discharging at 5 degrees
static const float p0_Discharge5 = 3.804;
static const float p1_Discharge5 = -0.3487;
static const float p2_Discharge5 = 8.383*0.00001;
static const float alpha1_Discharge5 = -0.001618;
static const float alpha2_Discharge5 = -0.04724;
// Charging at 15 degrees
static const float p0_Charge15 = 3.629;
static const float p1_Charge15 = -0.3191;
static const float p2_Charge15 = 6.952*0.00001;
static const float alpha1_Charge15 = -0.0005411;
static const float alpha2_Charge15 = -0.1493;
// Discharging at 15 degrees
static const float p0_Discharge15 = 3.599;
static const float p1_Discharge15 = -0.2933;
static const float p2_Discharge15 = 6.9*0.00001;
static const float alpha1_Discharge15 = -0.0004687;
static const float alpha2_Discharge15 = -0.1493;
// Charging at 25 degrees
static const float p0_Charge25 = 3.637;
static const float p1_Charge25 = -0.3091;
static const float p2_Charge25 = 7.033*0.00001;
static const float alpha1_Charge25 = -0.0005747;
static const float alpha2_Charge25 = -0.1366;
// Discharging at 25 degrees
static const float p0_Discharge25 = 3.604;
static const float p1_Discharge25 = -0.2803;
static const float p2_Discharge25 = 6.871*0.00001;
static const float alpha1_Discharge25 = -0.0004523;
static const float alpha2_Discharge25 = -0.1341;
// Charging at 45 degrees 
static const float p0_Charge45 = 3.584;
static const float p1_Charge45 = -0.4868;
static const float p2_Charge45 = 6.212*0.00001;
static const float alpha1_Charge45 = -0.0001919;
static const float alpha2_Charge45 = -0.2181;
// Discharging at 45 degrees
static const float p0_Discharge45 = 3.55;
static const float p1_Discharge45 = -0.4573;
static const float p2_Discharge45 = 6.02*0.00001;
static const float alpha1_Discharge45 = -6.241*0.00001;
static const float alpha2_Discharge45 = -0.221;

void socInit(void);
void socProcess(void);
void updateBattCapacity();
void updateSOC(TickType_t xWakeTime, float current);
float getSOC(void);
float getSOCFromVoltage(float voltage, float temp, float estimatedSOC);
float getOCV(float SOC, float temp);
float getInstantaneousTemp(void);
float getInstantaneousVoltage(void);
float getInstantaneousCurrent(void);
float getBattLevel(void);
float getBattCapacity(void);
void calculateImpedance(float OCV, float voltage, float current);
void calculateRemainingRunTime(float battLevel, float current);


#endif /* PHANTOM_DRIVERS_INCLUDE_SOC_H_ */
