//
// Created by yan on 7/26/26.
//

#ifndef NUCLEO_RTOS_PROJECT_APP_H
#define NUCLEO_RTOS_PROJECT_APP_H
#include "queues.h"
#include "mutex.h"
#include "../../Interfaces/Inc/SpiDevice.h"
#include "readSensorData_task.h"
#include "showSensorData_task.h"
#include "showErrorData_task.h"
#include "../../Interfaces/Inc/BMP280.h"
#include "../../Interfaces/Inc/Spi.h"
#include "../../Interfaces/Inc/Lcd.h"
#include "../../Interfaces/Inc/Utilities.h"


// App init function

void appInit(void);



#endif //NUCLEO_RTOS_PROJECT_APP_H
