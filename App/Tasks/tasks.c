//
// Created by yan on 7/26/26.
//

#include "tasks.h"

#include "readSensorData_task.h"
#include "showSensorData_task.h"


void createTasks(void)
{
    createTaskReadSensorData();
    createTaskShowSensorData();

}
