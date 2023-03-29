/*
 * File:   bme280.c
 * Author: C16783
 *
 * Created on May 13, 2020, 11:56 AM
 */

#include "bme280.h"
#include "TWI.h"

#include <math.h>
#include <util/delay.h>

//Global BME 280 Variables    
char str_temp[30];
char str_press[30];
char str_hum[30];
bme280_config_t bme280_config;
uint8_t bme280_ctrl_hum;
bme280_ctrl_meas_t bme280_ctrl_meas;
bme280_calibration_param_t calibParam;
long adc_T, adc_H, adc_P, t_fine;
bool weather_initialized = 0;

/**
  Section: Driver APIs
 */
//#define DEBUGGING     // Uncomment to view debug statements

bool BME280_getID(void) {
    uint8_t bme280_chip_id[1];
    bool successRX;
    successRX = TWI_sendAndReadBytes(BME280_ADDR, BME280_ID_REG, &bme280_chip_id[0], 1);
#ifdef DEBUGGING
    printf("Reading BME280 sensor chip-id: %x\n", bme280_chip_id[0]);
#endif
    return successRX;
    
}

bool BME280_reset(void) {
#ifdef DEBUGGING
    printf("Resetting BME280 sensor.\n");
#endif
    uint8_t bme280_soft_reset_command[] = {BME280_RESET_REG, BME280_SOFT_RESET};
    bool successTX = TWI_sendBytes(BME280_ADDR,&bme280_soft_reset_command[0],2);
    return successTX;
}

bool BME280_readFactoryCalibrationParams(void) {
#ifdef DEBUGGING 
    printf("Parsing BME280 calibration data.\n");
#endif
    uint8_t paramBuff[24];
    bool successRX;
    successRX = TWI_sendAndReadBytes(BME280_ADDR, BME280_CALIB_DT1_LSB_REG, paramBuff, 24);
    calibParam.dig_T1 = (((uint16_t) paramBuff[1]) << 8) + paramBuff[0];
    calibParam.dig_T2 = (((int) paramBuff[3]) << 8) + paramBuff[2];
    calibParam.dig_T3 = (((int) paramBuff[5]) << 8) + paramBuff[4];
    calibParam.dig_P1 = (((uint16_t) paramBuff[7]) << 8) + paramBuff[6];
    calibParam.dig_P2 = (((int) paramBuff[9]) << 8) + paramBuff[8];
    calibParam.dig_P3 = (((int) paramBuff[11]) << 8) + paramBuff[10];
    calibParam.dig_P4 = (((int) paramBuff[13]) << 8) + paramBuff[12];
    calibParam.dig_P5 = (((int) paramBuff[15]) << 8) + paramBuff[14];
    calibParam.dig_P6 = (((int) paramBuff[17]) << 8) + paramBuff[16];
    calibParam.dig_P7 = (((int) paramBuff[19]) << 8) + paramBuff[18];
    calibParam.dig_P8 = (((int) paramBuff[21]) << 8) + paramBuff[20];
    calibParam.dig_P9 = (((int) paramBuff[23]) << 8) + paramBuff[22];

    successRX = TWI_sendAndReadBytes(BME280_ADDR, BME280_CALIB_DH1_REG, paramBuff, 1);
    calibParam.dig_H1 = (((uint8_t) paramBuff[1]) << 8) + paramBuff[0];

    successRX = TWI_sendAndReadBytes(BME280_ADDR, BME280_CALIB_DH2_LSB_REG, paramBuff, 7);
    calibParam.dig_H2 = (((int) paramBuff[1]) << 8) + paramBuff[0];
    calibParam.dig_H3 = (uint8_t) paramBuff[2];
    calibParam.dig_H4 = (((int) paramBuff[3]) << 4) | (paramBuff[4] & 0xF);
    calibParam.dig_H5 = (((int) paramBuff[5]) << 4) | (paramBuff[4] >> 4);
    calibParam.dig_H6 = (short) paramBuff[6];
    return successRX;
}

void BME280_config(uint8_t sbtime, uint8_t coeff) {
#ifdef DEBUGGING 
    printf("Setting standby time and filter coefficient.\n");
#endif
    bme280_config.t_sb = sbtime; // Set standby time;
    bme280_config.filter = coeff; // Set filter coefficient;
}

void BME280_ctrl_meas(uint8_t osrs_T, uint8_t osrs_P, uint8_t mode) {
#ifdef DEBUGGING 
    printf("Setting oversampling temperature and pressure, sensor mode.\n");
#endif
    bme280_ctrl_meas.osrs_T = osrs_T; // Set oversampling temperature;
    bme280_ctrl_meas.osrs_P = osrs_P; // Set oversampling pressure;
    bme280_ctrl_meas.mode = mode; // Set sensor mode;
}

void BME280_ctrl_hum(uint8_t osrs_H) {
#ifdef DEBUGGING 
    printf("Setting oversampling humidity.\n");
#endif
    bme280_ctrl_hum = osrs_H; // Set oversampling humidity;
}

void BME280_initializeSensor(void) {
#ifdef DEBUGGING 
    printf("Initializing sensor.\n");
#endif
    uint8_t command_1[] = {BME280_CONFIG_REG, bme280_config.configReg};
    TWI_sendBytes(BME280_ADDR, &command_1[0], 2);
    uint8_t command_2[] = {BME280_CTRL_HUM_REG, bme280_ctrl_hum};
    TWI_sendBytes(BME280_ADDR, &command_2[0], 2);
    uint8_t command_3[] = {BME280_CTRL_MEAS_REG, bme280_ctrl_meas.ctrlMeasReg};
    TWI_sendBytes(BME280_ADDR, &command_3[0], 2);
}

void BME280_startForcedSensing(void) {
#ifdef DEBUGGING 
    printf("Starting forced sensing.\n");
#endif
    bme280_ctrl_meas.mode = BME280_FORCED_MODE;
    uint8_t command[] = {BME280_CTRL_MEAS_REG, bme280_ctrl_meas.ctrlMeasReg};
    TWI_sendBytes(BME280_ADDR, &command[0], 2);
}

void BME280_readMeasurements(void) {
#ifdef DEBUGGING 
    printf("Reading measurements.\n");
#endif
    uint8_t sensorData[BME280_DATA_FRAME_SIZE];

    TWI_sendAndReadBytes(BME280_ADDR, BME280_PRESS_MSB_REG, &sensorData[0], BME280_DATA_FRAME_SIZE);

    adc_H = ((uint32_t) sensorData[BME280_HUM_MSB] << 8) |
            sensorData[BME280_HUM_LSB];

    adc_T = ((uint32_t) sensorData[BME280_TEMP_MSB] << 12) |
            (((uint32_t) sensorData[BME280_TEMP_LSB] << 4) |
            ((uint32_t) sensorData[BME280_TEMP_XLSB] >> 4));

    adc_P = ((uint32_t) sensorData[BME280_PRESS_MSB] << 12) |
            (((uint32_t) sensorData[BME280_PRESS_LSB] << 4) |
            ((uint32_t) sensorData[BME280_PRESS_XLSB] >> 4));
}

float BME280_getTemperature(void) {
    float temperature = (float) BME280_compensateTemperature() / 100;
    return temperature;
}

float BME280_getPressure(void) {
    float pressure = (float) BME280_compensatePressure() / 100; // measured in hPa (equivalent to millibar)
 
    // Note: Atmospheric pressure changes with elevation. 
    // The following code finds the equivalent pressure at sea level to give accurate readings
    // in accordance with the international Standard Atmosphere.
    // The equation is: P0 = P1 (1 - (0.0065h/ (T + 0.0065h + 273.15))^(-5.257)
    // where:   P0 = calculated mean sea level pressure (hPa)   
    //          P1 = actual measured pressure (hPa))
    //          h = elevation (m) 
    //          T = temp is degrees C
    float temp = BME280_getTemperature();
    double mantissa = 1 - (0.0065 * ELEVATION / (temp + (0.0065 * ELEVATION) + 273.15));
    double adjustment = pow(mantissa, -5.257);
    float press_adj = adjustment * pressure;

    return press_adj;
}

float BME280_getHumidity(void) {
    float humidity = (float) BME280_compensateHumidity() / 1024;
    return humidity;
}

/* 
 * Returns temperature in DegC, resolution is 0.01 DegC. 
 * Output value of "5123" equals 51.23 DegC.  
 */
uint32_t BME280_compensateTemperature(void) {
    long tempV1, tempV2, t;

    tempV1 = ((((adc_T >> 3) - ((long) calibParam.dig_T1 << 1))) * ((long) calibParam.dig_T2)) >> 11;
    tempV2 = (((((adc_T >> 4) - ((long) calibParam.dig_T1)) * ((adc_T >> 4) - ((long) calibParam.dig_T1))) >> 12)*((long) calibParam.dig_T3)) >> 14;
    t_fine = tempV1 + tempV2;
    t = (t_fine * 5 + 128) >> 8;

    return t;
}

/* 
 * Returns pressure in Pa as unsigned 32 bit integer. 
 * Output value of "96386" equals 96386 Pa = 96.386 kPa 
 */
uint32_t BME280_compensatePressure(void) {
    long pressV1, pressV2;
    uint32_t p;

    pressV1 = (((long) t_fine) >> 1) - (long) 64000;
    pressV2 = (((pressV1 >> 2) * (pressV1 >> 2)) >> 11) * ((long) calibParam.dig_P6);
    pressV2 = pressV2 + ((pressV1 * ((long) calibParam.dig_P5)) << 1);
    pressV2 = (pressV2 >> 2)+(((long) calibParam.dig_P4) << 16);
    pressV1 = (((calibParam.dig_P3 * (((pressV1 >> 2) * (pressV1 >> 2)) >> 13)) >> 3) +
            ((((long) calibParam.dig_P2) * pressV1) >> 1)) >> 18;
    pressV1 = ((((32768 + pressV1))*((long) calibParam.dig_P1)) >> 15);

    if (pressV1 == 0) {
        // avoid exception caused by division by zero
        return 0;
    }

    p = (((uint32_t) (((long) 1048576) - adc_P)-(pressV2 >> 12)))*3125;
    if (p < 0x80000000) {
        p = (p << 1) / ((uint32_t) pressV1);
    } else {
        p = (p / (uint32_t) pressV1) * 2;
    }

    pressV1 = (((long) calibParam.dig_P9) * ((long) (((p >> 3) * (p >> 3)) >> 13))) >> 12;
    pressV2 = (((long) (p >> 2)) * ((long) calibParam.dig_P8)) >> 13;
    p = (uint32_t) ((long) p + ((pressV1 + pressV2 + calibParam.dig_P7) >> 4));

    return p;
}

uint32_t BME280_compensateHumidity(void) {
    long humV;
    uint32_t h;

    humV = (t_fine - ((long) 76800));
    humV = (((((adc_H << 14) - (((long) calibParam.dig_H4) << 20) - (((long) calibParam.dig_H5) * humV)) +
            ((long) 16384)) >> 15) * (((((((humV * ((long) calibParam.dig_H6)) >> 10) *
            (((humV * ((long) calibParam.dig_H3)) >> 11) + ((long) 32768))) >> 10) +
            ((long) 2097152)) * ((long) calibParam.dig_H2) + 8192) >> 14));
    humV = (humV - (((((humV >> 15) * (humV >> 15)) >> 7) * ((long) calibParam.dig_H1)) >> 4));
    humV = (humV < 0 ? 0 : humV);
    humV = (humV > 419430400 ? 419430400 : humV);

    h = (uint32_t) (humV >> 12);
    return h;
}

bool BME280_init(void) {
    printf("\nInitializing BME280...\n");
    BME280_reset();
    _delay_ms(50);
    BME280_readFactoryCalibrationParams();
    BME280_config(BME280_STANDBY_HALFMS, BME280_FILTER_COEFF_OFF);
    BME280_ctrl_meas(BME280_OVERSAMP_X1, BME280_OVERSAMP_X1, BME280_FORCED_MODE);
    BME280_ctrl_hum(BME280_OVERSAMP_X1);
    BME280_initializeSensor();
    printf("BME280 initialized\n");
    return true;
}

