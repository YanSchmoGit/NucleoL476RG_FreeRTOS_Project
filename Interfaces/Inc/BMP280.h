//
// Created by yan on 9/8/26.
//

#ifndef NUCLEO_RTOS_PROJECT_BMP280_H
#define NUCLEO_RTOS_PROJECT_BMP280_H
#include <stdint.h>

// ##### Register definitions #####

#define BMP280_REGISTER_CALIB_00			(0x88)   // Calibration Data 00
#define BMP280_REGISTER_CALIB_01			(0x89)   // Calibration Data 01
#define BMP280_REGISTER_CALIB_02			(0x8A)   // Calibration Data 02
#define BMP280_REGISTER_CALIB_03			(0x8C)   // Calibration Data 03
#define BMP280_REGISTER_CALIB_04			(0x8D)   // Calibration Data 04
#define BMP280_REGISTER_CALIB_05			(0x8E)   // Calibration Data 05
#define BMP280_REGISTER_CALIB_06			(0x8F)   // Calibration Data 06
#define BMP280_REGISTER_CALIB_07			(0x90)   // Calibration Data 07
#define BMP280_REGISTER_CALIB_08			(0x91)   // Calibration Data 08
#define BMP280_REGISTER_CALIB_09			(0x92)   // Calibration Data 09
#define BMP280_REGISTER_CALIB_10			(0x93)   // Calibration Data 10
#define BMP280_REGISTER_CALIB_11			(0x94)   // Calibration Data 11
#define BMP280_REGISTER_CALIB_12			(0x95)   // Calibration Data 12
#define BMP280_REGISTER_CALIB_13			(0x96)   // Calibration Data 13
#define BMP280_REGISTER_CALIB_14			(0x97)   // Calibration Data 14
#define BMP280_REGISTER_CALIB_15			(0x98)   // Calibration Data 15
#define BMP280_REGISTER_CALIB_16			(0x99)   // Calibration Data 16
#define BMP280_REGISTER_CALIB_17			(0x9A)   // Calibration Data 17
#define BMP280_REGISTER_CALIB_18			(0x9B)   // Calibration Data 18
#define BMP280_REGISTER_CALIB_19			(0x9C)   // Calibration Data 19
#define BMP280_REGISTER_CALIB_20			(0x9D)   // Calibration Data 20
#define BMP280_REGISTER_CALIB_21			(0x9E)   // Calibration Data 21
#define BMP280_REGISTER_CALIB_22			(0x9F)   // Calibration Data 22
#define BMP280_REGISTER_CALIB_23			(0xA0)   // Calibration Data 23 -- RESERVED
#define BMP280_REGISTER_CALIB_24			(0xA1)   // Calibration Data 24 -- RESERVED
#define BMP280_REGISTER_CALIB_25			(0xA2)   // Calibration Data 25 -- RESERVED

//Control/ Configure Register

#define BMP280_REGISTER_ID					(0xD0)   // Chip ID
#define BMP280_REGISTER_RESET				(0xE0)   // Reset
#define BMP280_REGISTER_STATUS				(0xF3)   // Measuring,  IM_Update
#define BMP280_REGISTER_CTRL_MEAS			(0xF4)   // OSRS_T, OSRS_P, MODE
#define BMP280_REGISTER_CONFIG				(0xF5)   // T_SB, FILTER, SPI3W_EN

// Data Register

#define BMP280_REGISTER_PRESS_MSB			(0xF7)   // PRESS_MSB
#define BMP280_REGISTER_PRESS_LSB			(0xF8)   // PRESS_LSB
#define BMP280_REGISTER_PRESS_XLSB			(0xF9)   // PRESS_XLSB
#define BMP280_REGISTER_TEMP_MSB			(0xFA)   // TEMP_MSB
#define BMP280_REGISTER_TEMP_LSB			(0xFB)   // TEMP_LSB
#define BMP280_REGISTER_TEMP_XLSB			(0xFC)   // TEMP_XLSB


// Calibation data

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

extern BMP280CalibrationData BMP280CalibData;

// BMP280 Datatypes

typedef int32_t BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t BMP280_S64_t;

typedef struct
{
	int32_t valueTemp;
	uint32_t valuePress;
} BMP280Values;


// Init data

#define BMP280_INIT_DATA					(0x27)

// ##### BMP280 Functions #####


// Init BMP280
void InitBMP280();

// Get calibration data
void GetSensorCalibrationData();

// Compensation functions

BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);


//  Process sensor data
void ProcessSensorData(BMP280Values *values, uint8_t *raw_data);


#endif //NUCLEO_RTOS_PROJECT_BMP280_H
