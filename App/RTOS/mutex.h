//
// Created by yan on 9/22/26.
//

#ifndef NUCLEO_RTOS_PROJECT_MUTEX_H
#define NUCLEO_RTOS_PROJECT_MUTEX_H
#include "cmsis_os2.h"

extern osMutexId_t lcdMutexHandle;

void createMutexes();

#endif //NUCLEO_RTOS_PROJECT_MUTEX_H
