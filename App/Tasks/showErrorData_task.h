//
// Created by yan on 9/16/26.
//

#ifndef NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
#define NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
#include "cmsis_os2.h"

#define ERROR_HANDLE_NO_DATA_IN_QUEUE       (0x01)
#define ERROR_HANDLE_VALUES_OUT_OF_BOUNDS   (0x02)
#define ERROR_HANDLE_DMA1_TRANSFER_ERROR    (0x03)
#define ERROR_HANDLE_SPI1_CRC_ERROR         (0x04)
#define ERROR_HANDLE_SPI1_OVERRUN_ERROR     (0x05)
#define ERROR_HANDLE_SPI1_MODE_FAULT_ERROR  (0x05)


extern osThreadId_t showErrorDataHandle;

// Create task function
void createTaskShowErrorData(void);

#endif //NUCLEO_RTOS_PROJECT_SHOWERRORDATA_TASK_H
