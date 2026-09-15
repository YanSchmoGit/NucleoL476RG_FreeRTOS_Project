//
// Created by yan on 9/8/26.
//

#include "../Inc/BMP280.h"

#include "../Inc/Peripherals.h"

BMP280CalibrationData BMP280CalibData;



void InitBMP280()
{

    // Send init data to BMP280
    WriteSpiData(BMP280_REGISTER_CTRL_MEAS, BMP280_INIT_DATA);

    // Get calibration data

    GetSensorCalibrationData();

}

void GetSensorCalibrationData()
{

    static uint8_t tempData[23];

    // Get data from sensor
    ReadSpiData(BMP280_REGISTER_CALIB_00, 23, tempData);

    BMP280CalibData.dig_T1 = (int16_t)(((uint16_t)tempData[1] << 8) | tempData[0] );
    BMP280CalibData.dig_T2 = (((uint16_t)tempData[3] << 8) | tempData[2] );
    BMP280CalibData.dig_T3 = (((uint16_t)tempData[5] << 8) | tempData[4] );

    BMP280CalibData.dig_P1 = (int16_t)(((uint16_t)tempData[7] << 8) | tempData[6] );
    BMP280CalibData.dig_P2 = (((uint16_t)tempData[9] << 8) | tempData[8] );
    BMP280CalibData.dig_P3 = (((uint16_t)tempData[11] << 8) | tempData[10] );
    BMP280CalibData.dig_P4 = (((uint16_t)tempData[13] << 8) | tempData[12] );
    BMP280CalibData.dig_P5 = (((uint16_t)tempData[15] << 8) | tempData[14] );
    BMP280CalibData.dig_P6 = (((uint16_t)tempData[17] << 8) | tempData[16] );
    BMP280CalibData.dig_P7 = (((uint16_t)tempData[19] << 8) | tempData[18] );
    BMP280CalibData.dig_P8 = (((uint16_t)tempData[21] << 8) | tempData[20] );
    BMP280CalibData.dig_P9 = (((uint16_t)tempData[23] << 8) | tempData[22] );

};

BMP280_S32_t t_fine;
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)BMP280CalibData.dig_T1 << 1))) * ((BMP280_S32_t)BMP280CalibData.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)BMP280CalibData.dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)BMP280CalibData.dig_T1))) >> 12) * ((BMP280_S32_t)BMP280CalibData.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)BMP280CalibData.dig_P6;
    var2 = var2 + ((var1*(BMP280_S64_t)BMP280CalibData.dig_P5)<<17);
    var2 = var2 + (((BMP280_S64_t)BMP280CalibData.dig_P4)<<35);
    var1 = ((var1 * var1 * (BMP280_S64_t)BMP280CalibData.dig_P3)>>8) + ((var1 * (BMP280_S64_t)BMP280CalibData.dig_P2)<<12);
    var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)BMP280CalibData.dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BMP280_S64_t)BMP280CalibData.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BMP280_S64_t)BMP280CalibData.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)BMP280CalibData.dig_P7)<<4);
    return (BMP280_U32_t)p;
};



void ProcessSensorData(BMP280Values *values, uint8_t *raw_data)
{
    static uint32_t tempValuePress;
    static int32_t tempValueTemp;

    // Merge data to raw data variables
    tempValuePress = (uint32_t)((raw_data[1] << 12) | (raw_data[2] << 4) | (raw_data[3] >> 4));
    tempValueTemp = (int32_t)((raw_data[4] << 12) | (raw_data[5] << 4) | (raw_data[6] >> 4));

    values->valuePress = (bmp280_compensate_P_int64(tempValuePress) / 256);
    values->valueTemp = bmp280_compensate_T_int32(tempValueTemp);

};