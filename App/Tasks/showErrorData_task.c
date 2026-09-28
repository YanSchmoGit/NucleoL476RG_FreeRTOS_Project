//
// Created by yan on 9/16/26.
//

#include "showErrorData_task.h"
#include "cmsis_os2.h"
#include "queues.h"
#include "mutex.h"
#include "../../Interfaces/Inc/Lcd.h"

/* Definitions for showErrorData */
osThreadId_t showErrorDataHandle;
const osThreadAttr_t showErrorData_attributes = {
    .name = "showErrorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void startShowErrorData(void* argument);

void createTaskShowErrorData(void)
{
    /* creation of showErrorData */
    showErrorDataHandle = osThreadNew(startShowErrorData, NULL, &showErrorData_attributes);
}

// Task loop

void startShowErrorData(void* argument)
{
    uint32_t flags;


    /* Infinite loop */
    for (;;)
    {
        flags = osThreadFlagsWait(ERROR_HANDLE_NO_DATA_IN_QUEUE | ERROR_HANDLE_VALUES_OUT_OF_BOUNDS, osFlagsWaitAny,
                                  osWaitForever);

        //Acquire lcd mutex
        osStatus_t lcdMutexStatus = osMutexAcquire(lcdMutexHandle, osWaitForever);

        if (lcdMutexStatus == osOK)
        {

            SetLcdCursorPosition(0, 0);
            SendLcdString("Error:           ");

            if (flags == ERROR_HANDLE_NO_DATA_IN_QUEUE)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("No data in queue       ");
            }
            else if (flags == ERROR_HANDLE_VALUES_OUT_OF_BOUNDS)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("Value bounds     ");
            }
            else if (flags == ERROR_HANDLE_DMA1_TRANSFER_ERROR)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("DMA1 transfer error    ");
            }
            else if (flags == ERROR_HANDLE_SPI1_CRC_ERROR)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("SPI1 CRC error    ");
            }
            else if (flags == ERROR_HANDLE_SPI1_OVERRUN_ERROR)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("Overrun error    ");
            }
            else if (flags == ERROR_HANDLE_SPI1_MODE_FAULT_ERROR)
            {
                SetLcdCursorPosition(0, 1);
                SendLcdString("Mode fault error    ");
            }


            osMutexRelease(lcdMutexHandle);
        }


        osDelay(10);
    }
}
