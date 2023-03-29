/* 
 * File:   bme280.h
 * Author: C16783
 *
 * Created on May 13, 2020, 11:56 AM
 */

#ifndef BME280_H
#define	BME280_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "mcc_generated_files/system/system.h"
#include <stdbool.h>    

    /**
  Section: Macro Declarations
     */

// Device Elevation
#define ELEVATION 366       // in meters
    
// Device Information
#define BME280_ADDR     			0x76
#define BME280_CHIP_ID  			0x60

// Sensor Modes
#define BME280_SLEEP_MODE           0x00
#define BME280_FORCED_MODE          0x01
#define BME280_NORMAL_MODE          0x03
#define BME280_SOFT_RESET           0xB6

// Oversampling Options
#define BME280_OVERSAMP_SKIPPED     0x00
#define BME280_OVERSAMP_X1          0x01
#define BME280_OVERSAMP_X2          0x02
#define BME280_OVERSAMP_X4          0x03
#define BME280_OVERSAMP_X8          0x04
#define BME280_OVERSAMP_X16         0x05

// Standby Time
#define BME280_STANDBY_HALFMS   	0x00
#define BME280_STANDBY_63MS     	0x01
#define BME280_STANDBY_125MS    	0x02
#define BME280_STANDBY_250MS    	0x03
#define BME280_STANDBY_500MS    	0x04
#define BME280_STANDBY_1000MS   	0x05
#define BME280_STANDBY_10MS     	0x06
#define BME280_STANDBY_20MS     	0x07

// Filter Coefficients
#define BME280_FILTER_COEFF_OFF     0x00
#define BME280_FILTER_COEFF_2       0x01
#define BME280_FILTER_COEFF_4       0x02
#define BME280_FILTER_COEFF_8       0x03
#define BME280_FILTER_COEFF_16      0x04

// Register Addresses
#define BME280_ID_REG           0xD0 
#define BME280_RESET_REG        0xE0
#define BME280_CTRL_HUM_REG     0xF2
#define BME280_STATUS_REG       0xF3 
#define BME280_CTRL_MEAS_REG    0xF4   
#define BME280_CONFIG_REG       0xF5 
#define BME280_PRESS_MSB_REG    0xF7
#define BME280_PRESS_LSB_REG    0xF8
#define BME280_PRESS_XLSB_REG   0xF9 
#define BME280_TEMP_MSB_REG     0xFA
#define BME280_TEMP_LSB_REG     0xFB 
#define BME280_TEMP_XLSB_REG    0xFC 
#define BME280_HUM_MSB_REG      0xFD  
#define BME280_HUM_LSB_REG      0xFE

// Sensor Data Read Bytes
#define BME280_PRESS_MSB        0
#define BME280_PRESS_LSB        1
#define BME280_PRESS_XLSB       2
#define BME280_TEMP_MSB         3
#define BME280_TEMP_LSB         4
#define BME280_TEMP_XLSB        5
#define BME280_HUM_MSB          6
#define BME280_HUM_LSB          7
#define BME280_DATA_FRAME_SIZE  8

// Factory Calibration Parameters
#define BME280_CALIB_DT1_LSB_REG    0x88
#define BME280_CALIB_DT1_MSB_REG    0x89
#define BME280_CALIB_DT2_LSB_REG    0x8A
#define BME280_CALIB_DT2_MSB_REG    0x8B
#define BME280_CALIB_DT3_LSB_REG    0x8C
#define BME280_CALIB_DT3_MSB_REG    0x8D
#define BME280_CALIB_DP1_LSB_REG    0x8E
#define BME280_CALIB_DP1_MSB_REG    0x8F
#define BME280_CALIB_DP2_LSB_REG    0x90
#define BME280_CALIB_DP2_MSB_REG    0x91
#define BME280_CALIB_DP3_LSB_REG    0x92
#define BME280_CALIB_DP3_MSB_REG    0x93
#define BME280_CALIB_DP4_LSB_REG    0x94
#define BME280_CALIB_DP4_MSB_REG    0x95
#define BME280_CALIB_DP5_LSB_REG    0x96
#define BME280_CALIB_DP5_MSB_REG    0x97
#define BME280_CALIB_DP6_LSB_REG    0x98
#define BME280_CALIB_DP6_MSB_REG    0x99
#define BME280_CALIB_DP7_LSB_REG    0x9A
#define BME280_CALIB_DP7_MSB_REG    0x9B
#define BME280_CALIB_DP8_LSB_REG    0x9C
#define BME280_CALIB_DP8_MSB_REG    0x9D
#define BME280_CALIB_DP9_LSB_REG    0x9E
#define BME280_CALIB_DP9_MSB_REG    0x9F
#define BME280_CALIB_DH1_REG        0xA1
#define BME280_CALIB_DH2_LSB_REG    0xE1
#define BME280_CALIB_DH2_MSB_REG    0xE2
#define BME280_CALIB_DH3_REG        0xE3
#define BME280_CALIB_DH4_MSB_REG    0xE4
#define BME280_CALIB_DH4_LSB_REG    0xE5
#define BME280_CALIB_DH5_MSB_REG    0xE6
#define BME280_CALIB_DH6_REG        0xE7
    
#define DEFAULT_STANDBY_TIME    BME280_STANDBY_HALFMS
#define DEFAULT_FILTER_COEFF    BME280_FILTER_COEFF_OFF
#define DEFAULT_TEMP_OSRS       BME280_OVERSAMP_X1
#define DEFAULT_PRESS_OSRS      BME280_OVERSAMP_X1
#define DEFAULT_HUM_OSRS        BME280_OVERSAMP_X1
#define DEFAULT_SENSOR_MODE     BME280_FORCED_MODE    
    
    /**
    Section: Variable Definitions
     */

//    char str_temp[30];
//    char str_press[30];
//    char str_hum[30];

    typedef union {

        struct {
            uint8_t mode : 2;
            uint8_t osrs_P : 3;
            uint8_t osrs_T : 3;
        };
        uint8_t ctrlMeasReg;
    } bme280_ctrl_meas_t;

    typedef union {

        struct {
            uint8_t spi3w_en : 1;
            uint8_t filter : 3;
            uint8_t t_sb : 3;
        };
        uint8_t configReg;
    } bme280_config_t;

    typedef struct {
        uint16_t dig_T1;
        int dig_T2;
        int dig_T3;
        uint16_t dig_P1;
        int dig_P2;
        int dig_P3;
        int dig_P4;
        int dig_P5;
        int dig_P6;
        int dig_P7;
        int dig_P8;
        int dig_P9;
        uint8_t dig_H1;
        int dig_H2;
        uint8_t dig_H3;
        int dig_H4;
        int dig_H5;
        signed char dig_H6;
    } bme280_calibration_param_t;



    /**
      Section: BME280 Sensor Driver APIs
     */

    bool BME280_getID(void);
    bool BME280_reset(void);
    bool BME280_readFactoryCalibrationParams(void);
    void BME280_config(uint8_t sbtime, uint8_t coeff);
    void BME280_ctrl_meas(uint8_t osrs_T, uint8_t osrs_P, uint8_t mode);
    void BME280_ctrl_hum(uint8_t osrs_H);
    void BME280_initializeSensor(void);
    void BME280_startForcedSensing(void);
    void BME280_readMeasurements(void);
    float BME280_getTemperature(void);
    float BME280_getPressure(void);
    float BME280_getHumidity(void);
    uint32_t BME280_compensateTemperature(void);
    uint32_t BME280_compensatePressure(void);
    uint32_t BME280_compensateHumidity(void);
    bool BME280_init(void);
    void BME280_readSensors(void);


#ifdef	__cplusplus
}
#endif

#endif	/* BME280_H */

