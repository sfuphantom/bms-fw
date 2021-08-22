/*
 * task_balance.h
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#ifndef PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_BALANCE_H_
#define PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_BALANCE_H_

/***********************************************************
 * @function                - vBalanceTask
 *
 * @brief                   - This task will passively balance the cells during BMS_CHARGING state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vBalanceTask(void *);              // This task will perform passive cell balancing


#endif /* PHANTOM_DRIVERS_TASKS_INCLUDE_TASK_BALANCE_H_ */
