//
// Created by yan on 7/26/26.
//

#include "readSensorData_task.h"

#include "cmsis_os2.h"
#include "stm32l476xx.h"

/* Definitions for readSensorData */
osThreadId_t readSensorDataHandle;
const osThreadAttr_t readSensorData_attributes = {
    .name = "readSensorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityLow,
  };

void startReadSensorData(void *argument);

void createTaskReadSensorData(void)
{
    /* creation of readSensorData */
    readSensorDataHandle = osThreadNew(startReadSensorData, NULL, &readSensorData_attributes);
}

// Task loop
void startReadSensorData(void *argument)
{

    // Test blink LED
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk);
    GPIOA->MODER |= GPIO_MODER_MODE5_0;

    // Test status variable
    uint8_t LEDstate = 0;



    /* Infinite loop */
    for(;;)
    {
        if(LEDstate == 0)
        {
            GPIOA->BSRR |= GPIO_BSRR_BS5;
            LEDstate = 1;
        }
        else
        {
            GPIOA->BSRR |= GPIO_BSRR_BR5;
            LEDstate = 0;
        }



        osDelay(1000);
    }

}