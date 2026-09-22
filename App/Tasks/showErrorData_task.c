//
// Created by yan on 9/16/26.
//

#include "showErrorData_task.h"
#include "cmsis_os2.h"
#include "queues.h"

/* Definitions for showErrorData */
osThreadId_t showErrorDataHandle;
const osThreadAttr_t showErrorData_attributes = {
    .name = "showErrorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void startShowErrorData(void* argument);

void createTaskShowErrorData(void)
{
    /* creation of showErrorData */
    showErrorDataHandle = osThreadNew(startShowErrorData, NULL, &showErrorData_attributes);
}

// Task loop

void startShowErrorData(void* argument)
{



    /* Infinite loop */
    for (;;)
    {

        osDelay(1);
    }
}