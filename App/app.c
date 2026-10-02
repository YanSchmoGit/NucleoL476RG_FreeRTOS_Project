//
// Created by yan on 7/26/26.
//

#include "app.h"

static BMP280Handle sensor_handle_BMP280_1;

void appInit(void)
{



    // Enable utilies
    EnableLcdTimer();
    WaitTime_ms(10);

    // Configure SPI interface
    ConfigSpiInterface();

    // Configure BMP280 Sensor
    InitBMP280(&sensor_handle_BMP280_1);

    // Configure LCD screen
    ConfigLcdScreen();
    InitializeLcdScreen();


    // Create queues
    createQueues();

    // Create mutexes
    createMutexes();

    // Create tasks
    createTaskReadSensorData(&sensor_handle_BMP280_1);
    createTaskShowSensorData();
    createTaskShowErrorData();
}
