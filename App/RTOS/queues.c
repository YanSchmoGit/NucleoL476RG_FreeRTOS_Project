//
// Created by yan on 7/26/26.
//

#include "queues.h"
#include "../Interfaces/Inc/BMP280.h"
#include "cmsis_os2.h"


/* Definitions for sesnsorData */

osMessageQueueId_t sensorDataHandle;
osMessageQueueId_t sensorErrorHandle;

const osMessageQueueAttr_t sensorData_attributes = {
    .name = "sensorData"
};

const osMessageQueueAttr_t sensorError_attributes = {
    .name = "sensorError"
};


void createQueues(void)
{
    /* Create the queue(s) */
    /* creation of sensorData */
    sensorDataHandle = osMessageQueueNew(16, sizeof(BMP280Values), &sensorData_attributes);
    sensorErrorHandle = osMessageQueueNew(16, sizeof(uint8_t), &sensorError_attributes);
}

