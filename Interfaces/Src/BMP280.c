//
// Created by yan on 9/8/26.
//

#include "../Inc/BMP280.h"

#include "../Inc/Spi.h"
#include "../Inc/Utilities.h"




void InitBMP280(BMP280Handle* sensor_handle)
{
    uint8_t bmp_id = 0;
    uint8_t bmp_ctrl = 0;

    // Send init data to BMP280


    // Attempts for initialization of sensor
    for (uint8_t i = 0; 1 < 5; i++)
    {
        WriteSpiData(BMP280_REGISTER_RESET, 0xB6);
        WaitTime_ms(5);
        WriteSpiData(BMP280_REGISTER_CTRL_MEAS, BMP280_INIT_DATA);
        WaitTime_ms(5);

        // Check for correct init values
        ReadSpiData(BMP280_REGISTER_ID, 1, &bmp_id);
        ReadSpiData(BMP280_REGISTER_CTRL_MEAS, 1, &bmp_ctrl);

        if ((bmp_id == BMP280_ID) & (bmp_ctrl == BMP280_INIT_DATA))
        {
            break;
        }
    }

    // Get calibration data
    GetSensorCalibrationData(sensor_handle);
}

void GetSensorCalibrationData(BMP280Handle* sensor_handle)
{
    static uint8_t tempData[24];

    // Get data from sensor
    ReadSpiData(BMP280_REGISTER_CALIB_00, 24, tempData);

    sensor_handle->CalibrationData.dig_T1 = (int16_t)(((uint16_t)tempData[1] << 8) | tempData[0]);
    sensor_handle->CalibrationData.dig_T2 = (((uint16_t)tempData[3] << 8) | tempData[2]);
    sensor_handle->CalibrationData.dig_T3 = (((uint16_t)tempData[5] << 8) | tempData[4]);

    sensor_handle->CalibrationData.dig_P1 = (int16_t)(((uint16_t)tempData[7] << 8) | tempData[6]);
    sensor_handle->CalibrationData.dig_P2 = (((uint16_t)tempData[9] << 8) | tempData[8]);
    sensor_handle->CalibrationData.dig_P3 = (((uint16_t)tempData[11] << 8) | tempData[10]);
    sensor_handle->CalibrationData.dig_P4 = (((uint16_t)tempData[13] << 8) | tempData[12]);
    sensor_handle->CalibrationData.dig_P5 = (((uint16_t)tempData[15] << 8) | tempData[14]);
    sensor_handle->CalibrationData.dig_P6 = (((uint16_t)tempData[17] << 8) | tempData[16]);
    sensor_handle->CalibrationData.dig_P7 = (((uint16_t)tempData[19] << 8) | tempData[18]);
    sensor_handle->CalibrationData.dig_P8 = (((uint16_t)tempData[21] << 8) | tempData[20]);
    sensor_handle->CalibrationData.dig_P9 = (((uint16_t)tempData[23] << 8) | tempData[22]);
};



BMP280_S32_t bmp280_compensate_T_int32(BMP280CalibrationData *calibration_data, BMP280_S32_t *t_fine, BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)calibration_data->dig_T1 << 1))) * ((BMP280_S32_t)calibration_data->dig_T2)) >>
        11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)calibration_data->dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)calibration_data->
        dig_T1))) >> 12) * ((BMP280_S32_t)calibration_data->dig_T3)) >> 14;
    (*t_fine) = var1 + var2;
    T = ((*t_fine) * 5 + 128) >> 8;
    return T;
}

BMP280_U32_t bmp280_compensate_P_int64(BMP280CalibrationData* calibration_data, BMP280_S32_t *t_fine, BMP280_S32_t adc_P)
{
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)*t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)calibration_data->dig_P6;
    var2 = var2 + ((var1 * (BMP280_S64_t)calibration_data->dig_P5) << 17);
    var2 = var2 + (((BMP280_S64_t)calibration_data->dig_P4) << 35);
    var1 = ((var1 * var1 * (BMP280_S64_t)calibration_data->dig_P3) >> 8) + ((var1 * (BMP280_S64_t)calibration_data->dig_P2)
        << 12);
    var1 = (((((BMP280_S64_t)1) << 47) + var1)) * ((BMP280_S64_t)calibration_data->dig_P1) >> 33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BMP280_S64_t)calibration_data->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BMP280_S64_t)calibration_data->dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)calibration_data->dig_P7) << 4);
    return (BMP280_U32_t)p;
};


void ProcessSensorData(BMP280Handle* sensor_handle, uint8_t* raw_data)
{
    static uint32_t tempValuePress;
    static int32_t tempValueTemp;

    // Merge data to raw data variables
    tempValuePress = (uint32_t)((raw_data[1] << 12) | (raw_data[2] << 4) | (raw_data[3] >> 4));
    tempValueTemp = (int32_t)((raw_data[4] << 12) | (raw_data[5] << 4) | (raw_data[6] >> 4));

    sensor_handle->SensorValues.valuePress = (bmp280_compensate_P_int64(&sensor_handle->CalibrationData, &sensor_handle->t_fine, (tempValuePress) / 256));
    sensor_handle->SensorValues.valueTemp = bmp280_compensate_T_int32(&sensor_handle->CalibrationData, &sensor_handle->t_fine, tempValueTemp);
};

uint8_t CheckSensorData(BMP280Values* values, uint32_t upperLimitTemp, uint32_t upperLimitPress,
                        uint32_t lowerLimitTemp, uint32_t lowerLimitPress)
{
    if ((values->valuePress < lowerLimitPress) | (values->valuePress > upperLimitPress) | (values->valueTemp < lowerLimitTemp)
        | (values->valueTemp > upperLimitTemp))
    {
        return 1;
    }
    else
    {
        return 0;
    }
};
