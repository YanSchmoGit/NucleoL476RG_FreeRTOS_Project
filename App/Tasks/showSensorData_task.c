//
// Created by yan on 7/26/26.
//

#include "showSensorData_task.h"

#include "cmsis_os2.h"

/* Definitions for showSensorData */
osThreadId_t showSensorDataHandle;
const osThreadAttr_t showSensorData_attributes = {
    .name = "showSensorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityLow,
  };

void startShowSensorData(void *argument);

void createTaskShowSensorData(void)
{
    /* creation of showSensorData */
    showSensorDataHandle = osThreadNew(startShowSensorData, NULL, &showSensorData_attributes);

}

// Task loop

void startShowSensorData(void *argument)
{

    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }

}