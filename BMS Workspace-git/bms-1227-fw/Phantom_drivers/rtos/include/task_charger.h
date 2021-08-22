/*
 * task_charger.h
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#ifndef PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_CHARGER_H_
#define PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_CHARGER_H_

/***********************************************************
 * @function                - vChargerTask
 *
 * @brief                   - This task runs functionto charge cells during BMS_CHARGING
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vChargerTask(void *pvParameters);  // This task will call function to charge cells


#endif /* PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_CHARGER_H_ */
