//
// Created by yan on 7/26/26.
//

#include "readSensorData_task.h"

#include "cmsis_os2.h"
#include "queues.h"
#include "stm32l476xx.h"
#include "showErrorData_task.h"
#include "../../Interfaces/Inc/Spi.h"
#include "../../Interfaces/Inc/BMP280.h"


/* Definitions for readSensorData */
osThreadId_t readSensorDataHandle;
const osThreadAttr_t readSensorData_attributes = {
    .name = "readSensorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void startReadSensorData(void* argument);

void createTaskReadSensorData(BMP280Handle* sensor)
{
    /* creation of readSensorData */
    readSensorDataHandle = osThreadNew(startReadSensorData, sensor, &readSensorData_attributes);
}


// Task loop
void startReadSensorData(void* argument)
{
    BMP280Handle* sensor = (BMP280Handle*)argument;

    static volatile uint8_t rx_data[7];


    /* Infinite loop */
    for (;;)
    {

        sensor->Device.readData(&readSensorDataHandle, rx_data,BMP280_REGISTER_PRESS_MSB, 7);
        ProcessSensorData(sensor, rx_data);



        // check sensor data
        if (CheckSensorData(&sensor->SensorValues, 4000, 110000, 0, 0) == 1)
        {
            // Bad values
            osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_VALUES_OUT_OF_BOUNDS);
        }
        else
        {
            // Good values
            osMessageQueuePut(sensorDataHandle, &sensor->SensorValues, 0, osWaitForever);
        }


        osDelay(10);
    }
}



