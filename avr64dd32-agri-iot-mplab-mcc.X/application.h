/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef APPLICATION_H
#define	APPLICATION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h> // include processor files - each processor file is guarded.
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "usart1.h"    
#include "bme280.h"
#include "TWI.h"    
#include "RN2xx3.h"
#include "ringBuffer.h"

// Moisture     
#define CONVERSION_PERCENT 0.0625   // 100 / (IN_MAX - IN_MIN)
#define IN_MAX 3000          
#define IN_MIN 1400

// Event Flags
#define UI_BUTTON_FLAG              (1 << 0)
#define TTN_JOIN_SUCCESS_FLAG       (1 << 1)
#define TX_SUCCESS_FLAG             (1 << 2)
#define TX_ERROR_FLAG               (1 << 3)
#define SLEEP_TIMER_FLAG            (1 << 4)

#define ALL_DATA    0
#define MOIST_ONLY  1

#define READ_INTERVAL 180;  //Time between sensor readings

typedef enum    {
    REGISTER,
    INIT,
    TTN_NOT_JOINED,
    TTN_JOIN,       
    TX_CNF,
    SLEEP,
    TX_UNCNF,
    TEST_MOISTURE,
    TEST_LORA,
    TEST_BME280,
    TEST_LOW_POWER,
    TEST_SENSOR_VALUES,
    WAIT_FOR_TEST
} STATE;

typedef struct {
    uint16_t min;
    uint16_t max;
} moist_range_t;

typedef struct
{
    int8_t temp;         // Air Temperature    Weather Click
    uint8_t humid;       // Humidity           Weather Click
    uint8_t moist;       // Soil Moisture      ADCC
    uint8_t press;      // Pressure           Weather Click   

} sensor_data_t;

typedef struct {
    uint16_t min;
    uint16_t max;
    uint16_t dry_air;
    uint16_t dry_soil;
    uint16_t wet_soil;
    uint16_t cup_water;
} soil_moist_t;

// Function Prototypes
void BUTTON_debounce(void);
void RTC_PIT_Callback(void);
void WeatherClick_initialize(void);
void WeatherClick_readSensors(void);
void printRawValue(void);
uint8_t map(uint16_t raw);
uint8_t getMoistureMeasurement(void);
void getSensorData(sensor_data_t *);
void printSensorData(sensor_data_t *);
void formatPayload(char *, sensor_data_t *);
uint8_t getNumTx(void);
void stateMachine(void);

#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_H */

