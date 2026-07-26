//
// Created by yan on 7/26/26.
//

#include "queues.h"

#include "cmsis_os2.h"


/* Definitions for sesnsorData */

osMessageQueueId_t sensorDataHandle;

const osMessageQueueAttr_t sensorData_attributes = {
    .name = "sesnsorData"
};


void createQueues(void)
{
    /* Create the queue(s) */
    /* creation of sesnsorData */
    sensorDataHandle = osMessageQueueNew(16, sizeof(uint32_t), &sensorData_attributes);
}
