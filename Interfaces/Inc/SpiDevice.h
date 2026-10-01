//
// Created by yan on 9/30/26.
//

#ifndef NUCLEO_RTOS_PROJECT_SPIDEVICE_H
#define NUCLEO_RTOS_PROJECT_SPIDEVICE_H
#include <stdint.h>

typedef int32_t BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t BMP280_S64_t;

typedef struct
{
    int32_t valueTemp;
    uint32_t valuePress;
} BMP280Values;

typedef struct {
    uint16_t 	dig_T1;
    int16_t 	dig_T2;
    int16_t 	dig_T3;
    uint16_t 	dig_P1;
    int16_t 	dig_P2;
    int16_t 	dig_P3;
    int16_t 	dig_P4;
    int16_t 	dig_P5;
    int16_t 	dig_P6;
    int16_t 	dig_P7;
    int16_t 	dig_P8;
    int16_t 	dig_P9;
} BMP280CalibrationData;

typedef struct
{
    BMP280CalibrationData CalibrationData;
    BMP280_S32_t t_fine;
    BMP280Values SensorValues;

}BMP280Handle;



#endif //NUCLEO_RTOS_PROJECT_SPIDEVICE_H
