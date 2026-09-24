//
// Created by yan on 7/26/26.
//

#include "showSensorData_task.h"

#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "queues.h"
#include "mutex.h"
#include "showErrorData_task.h"
#include "../../Interfaces/Inc/Lcd.h"
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

void startShowSensorData(void* argument)
{
    //InitializeLcdScreen();


    static BMP280Values data_queue;

    /* Infinite loop */
    for (;;)
    {

        if (osMessageQueueGet(sensorDataHandle, &data_queue, 0,500) == osOK)
        {
            //Acquire lcd mutex
            osStatus_t lcdMutexStatus = osMutexAcquire(lcdMutexHandle, osWaitForever);

            if (lcdMutexStatus == osOK)
            {
               SetLcdCursorPosition(0, 0);
                SendLcdString("Temp: ");
                SendLcdInteger(data_queue.valueTemp);
                SetLcdCursorPosition(0, 1);
                SendLcdString("Press: ");
                SendLcdInteger(data_queue.valuePress);

                osMutexRelease(lcdMutexHandle);
            }
        }
        else
        {
            osThreadFlagsSet(showErrorDataHandle, 0x01);

        }
        osDelay(100);
    }
}
