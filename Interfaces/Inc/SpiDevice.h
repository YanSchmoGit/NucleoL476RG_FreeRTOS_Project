//
// Created by yan on 9/30/26.
//

#ifndef NUCLEO_RTOS_PROJECT_SPIDEVICE_H
#define NUCLEO_RTOS_PROJECT_SPIDEVICE_H
#include <stdint.h>

// Struct for SPI device function pointer


typedef struct
{
    void (*readData)(void* ctx, uint8_t* data_rx, uint8_t reg, uint8_t length);
    void (*writeData)(void* ctx, uint8_t* data_tx, uint8_t reg, uint8_t length);
    void* ctx;
} SPIDevice_t;


#endif //NUCLEO_RTOS_PROJECT_SPIDEVICE_H
