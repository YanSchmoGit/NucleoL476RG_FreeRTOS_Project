//
// Created by yan on 9/16/26.
//

#ifndef NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
#define NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
#include "cmsis_os2.h"

extern osThreadId_t showErrorDataHandle;

// Create task function
void createTaskShowErrorData(void);

#endif //NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
