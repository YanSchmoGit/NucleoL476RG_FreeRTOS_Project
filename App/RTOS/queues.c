//
// Created by yan on 7/26/26.
//

#include "queues.h"
#include "../Interfaces/Inc/BMP280.h"
#include "cmsis_os2.h"


/* Definitions for sesnsorData */

osMessageQueueId_t sensorDataHandle;

const osMessageQueueAttr_t sensorData_attributes = {
    .name = "sensorData"
};


void createQueues(void)
{
    /* Create the queue(s) */
    /* creation of sensorData */
    sensorDataHandle = osMessageQueueNew(16, sizeof(BMP280Values), &sensorData_attributes);
}
