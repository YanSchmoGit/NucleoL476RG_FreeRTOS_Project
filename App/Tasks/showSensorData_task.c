//
// Created by yan on 7/26/26.
//

#include "showSensorData_task.h"

#include "cmsis_os2.h"
#include "queues.h"
#include "../../Interfaces/Inc/Peripherals.h"
#include "../../Interfaces/Inc/BMP280.h"

/* Definitions for showSensorData */
osThreadId_t showSensorDataHandle;
const osThreadAttr_t showSensorData_attributes = {
    .name = "showSensorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void startShowSensorData(void* argument);

void createTaskShowSensorData(void)
{
    /* creation of showSensorData */
    showSensorDataHandle = osThreadNew(startShowSensorData, NULL, &showSensorData_attributes);
}

// Task loop
static BMP280Values data_queue;
void startShowSensorData(void* argument)
{

    InitializeLcdScreen();



    /* Infinite loop */
    for (;;)
    {
        if (osMessageQueueGet(sensorDataHandle, &data_queue, 0,osWaitForever) == osOK)
        {


            SetLcdCursorPosition(0, 0);
            SendLcdString("Temp: ");
            SendLcdInteger(data_queue.valueTemp);
            SetLcdCursorPosition(0, 1);
            SendLcdString("Press: ");
            SendLcdInteger(data_queue.valuePress);
        }
        osDelay(1);
    }
}
