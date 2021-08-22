/*
 * task_sensorRead.h
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#ifndef PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_SENSORREAD_H_
#define PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_SENSORREAD_H_

/***********************************************************
 * @function                - vSensorReadTask
 *
 * @brief                   - This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vSensorReadTask(void *);           // This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)

#endif /* PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_SENSORREAD_H_ */
