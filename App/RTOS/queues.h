//
// Created by yan on 7/26/26.
//

#ifndef NUCLEO_RTOS_PROJECT_QUEUES_H
#define NUCLEO_RTOS_PROJECT_QUEUES_H
#include "cmsis_os2.h"

extern osMessageQueueId_t sensorDataHandle;
extern osMessageQueueId_t sensorErrorHandle;

void createQueues(void);


#endif //NUCLEO_RTOS_PROJECT_QUEUES_H
