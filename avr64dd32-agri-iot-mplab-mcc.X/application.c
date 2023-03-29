#include "application.h"
#include <stdio.h>
#include <util/delay.h>
#include "bme280.h"

/* Comment out to disable all debug print statements to save power */
#define DEBUGGING
/**
 * Section: Variable Definitions
 */

/* Counter for elapsed seconds (sleep timer) */
volatile uint32_t secs = 0;

/* RN2xx3 payload for TTN - String of hex characters representing sensor data */
char payload[11];

/* Data structure: measurements from Weather Click and moisture sensor */
static sensor_data_t data;

/* Structure for soil measurements */
//static moist_range_t soil;

/* Event Flags for User Interface */
static volatile uint16_t event_flags = 0;

/* State Machine Initial State: Change to test functionality, i.e. TEST_LORA.
 * Use INIT for normal operations */
static STATE state = INIT;

/* Keep track of the number of transmissions */
static volatile uint32_t numTx = 0;

/**
  Section: Driver APIs
 */

void BUTTON_debounce(void) {
    if(PORTF.IN & PIN6_bm) {
        // Turn off LED (Active Low))
        LED_SetHigh();
        event_flags |= UI_BUTTON_FLAG;
    }
    else
        LED_SetLow();
}

/*
 * Wakes the MCU
 */
void RTC_PIT_Callback(void) {
    secs++;
}

/* Prints a single raw soil moisture measurement */
void printRawValue() {
    uint16_t val;
    val = ADC0_GetConversion(ADC_MUXPOS_AIN2_gc);
    printf("%d\n", val);
}

uint8_t map(uint16_t raw) {    
    float val;
    
    // Account for random sporadic outlier values well out of range
    if(raw > IN_MAX) {
        raw = IN_MAX;
    }
    if(raw < IN_MIN) {
        raw = IN_MIN;
    }
    
    val = (raw - IN_MIN) * CONVERSION_PERCENT;

    return 100 - (uint8_t) val;
}

uint8_t getMoistureMeasurement() {
    uint16_t val = ADC0_GetConversion(ADC_MUXPOS_AIN2_gc);
    return map(val);    
}

void BME280_readSensors(void) {
    if (DEFAULT_SENSOR_MODE == BME280_FORCED_MODE) {
        BME280_startForcedSensing();
    }
    BME280_readMeasurements();
}

//Gets Data from the Weather Click and Soil Moisture Sensor
void getSensorData(sensor_data_t *data)
{
    BME280_readSensors();

    data->temp = (int8_t) BME280_getTemperature();
    data->press = BME280_getPressure() - 900;
    data->humid = (uint8_t) BME280_getHumidity();
    data->moist = getMoistureMeasurement(); 
}
/*
 * For Debug Purposes, prints retrieved data
 */
void printSensorData(sensor_data_t *data)
{
#ifdef DEBUGGING
    printf("\n\nCurrent Sensor Data\n");
    printf("-----------------------------------------------------------------------\n");
    
    printf("Relative Humidity: %u%% \r\n", data->humid);
    printf("         Moisture: %u%% \r\n", data->moist);
    printf("         Pressure: %u hPa\r\n", data->press + 900);
    printf("      Temperature: %i C \r\n", data->temp);
    printf("\nJSON Format\r\n");
    printf("-----------------------------------------------------------------------\n");
    printf("Payload: { temp: %i, humidity: %u, moisture: %u, pressure: %u }\r\n\n", 
            data->temp,
            data->humid, 
            data->moist, 
            data->press + 900
            );
#endif
}

/**
 * Converts Measurements to a Hex String for LoRa Transmission
 * Each measurement is converted to 2 hex chars, 2 nibbles making up
 * a single byte of data.  The payload is terminated with "\r\n\0"
 */ 
void formatPayload(char *str, sensor_data_t *data) {
        
    char hex[]= "0123456789ABCDEF";
    
    str[0] = hex[((data->temp >> 4) & 0x0F)];
    str[1] = hex[data->temp & 0x0F];
    str[2] = hex[((data->humid >> 4) & 0x0F)];
    str[3] = hex[data->humid & 0x0F];
    str[4] = hex[((data->moist >> 4) & 0x0F)];
    str[5] = hex[data->moist & 0x0F];
    str[6] = hex[((data->press >> 4) & 0x0F)];
    str[7] = hex[data->press & 0x0F];
    str[8] = '\r';
    str[9] = '\n';
    str[10] = '\0';

#ifdef DEBUGGING
    printf("Payload hex string: %snumTx: %lu\n", str, numTx);
#endif
}

void stateMachine()
{
    //Handle User Button Presses
    if(event_flags & UI_BUTTON_FLAG) {
        switch(state) {
            
            case TTN_NOT_JOINED:
                LED_SetHigh();
                state = TTN_JOIN;
                break;
            
            case WAIT_FOR_TEST:
                printRawValue();
                break;

            default:
                break;
        }
        event_flags &= ~UI_BUTTON_FLAG;
    }

    switch(state)   {
        case REGISTER:
            /* Gather necessary details to register on TTN */

            /* Initialize the LoRa Module */
            RN2xx3_init();
            
            /* Print the HWEUI to register with TTN */
            RN2xx3_getHWEUI();
            
#ifdef DEBUGGING
            printf("\r\nUse these details to register your device and generate keys with TTN\r\n");
#endif            
            /* Clear the Event_Flags */
            event_flags = 0;
            
            state = TTN_NOT_JOINED;
            break;
            
        case INIT:
            event_flags &= ~UI_BUTTON_FLAG;
            /* Turn off LED begin initialization processes */
            LED_SetHigh();

            /* Initialize the LoRa Module */
            RN2xx3_init();
            
            /* Print the HWEUI to register with TTN */
            RN2xx3_getHWEUI();

            /* Initialize the Weather Click */                     
            if(!BME280_init())
            {
#ifdef DEBUGGING
                printf("\nWeather Click NOT initialized properly!\n");
#endif
            }   
            /* Turn on LED to indicate initialization complete */ 
            LED_SetLow();
#ifdef DEBUGGING
            printf("\nPress button to join TTN.\n");
#endif
            
            /* Clear the Event_Flags */
            event_flags = 0;

            state = TTN_NOT_JOINED;  
            break;

        /* Remain here for a button press, active terminal interface allows user to
           test RN2xx3 state.  sys factoryRESET will reset the RN2xx3 and clear upctr */    
        case TTN_NOT_JOINED:
            RN2xx3_interface();
            break;

        case TTN_JOIN:
            /* Configure RN2xx3 with keys and settings */
            RN2xx3_config_TTN();
            
            /* Attempt to Join TTN */
            if(!RN2xx3_join_TTN())   {
                /* There was a problem joining TTN, do not attempt to transmit */
#ifdef DEBUGGING
                printf("Entering interactive terminal to troubleshoot.\r\n");
#endif
                RN2xx3_sys_RESET();              
                state = TTN_NOT_JOINED;
                break;
            }
            /* Join successful so transmit initial confirmed packet */
            else {
                state = TX_CNF;
            }
            break;
        
        /* Transmit a confirmed LoRa packet- confirmed messages require an ACK
         * and therefor consume more power.  Use unconfirmed messages wherever
         * possible. */    
        case TX_CNF:
            numTx++;
            /* Turn on LED to indicate begin tx*/
            LED_SetLow();
            
            /* Gather and format sensor data for transmission */
            getSensorData(&data);
            printSensorData(&data);
            formatPayload(payload, &data);
            if(!RN2xx3_tx_cnf(payload)){
#ifdef DEBUGGING
                printf("Tx unsuccessful, entering sleep.\r\n");
#endif
                
            }
            /* Put RN2xx3 to sleep, make sure the interval (in ms) is less than
             * the Tx interval (TX_INTERVAL_SEC) which is in seconds.
             * When RN2xx3  wakes up, either an 'ok' will appear in the rxBuffer
             * or 'invalid_param' when improperly formatted or too long */
            RN2xx3_cmd("sys sleep 90000\r\n");
            
            /* Start the PIT in preparation for MCU sleep */
            RTC_Start();
            state = SLEEP;
            LED_SetHigh();
            break;

        case SLEEP:
            if(secs == TX_INTERVAL_SEC) {
                //event_flags &= ~SLEEP_TIMER_FLAG;
                
                /* Once per day send confirmed packet (TX_CNF), otherwise send 
                 * packets unconfirmed (TX_UNCNF) 
                 * For interval 3 min, send 1 confirmed message every 480 messages */
                if(numTx % 480 == 0) {
                    state = TX_CNF;
                }
                else {
                    state = TX_UNCNF;
                }
                RTC_Stop();
                secs = 0;
            }
            /* The interactive terminal is active during SLEEP for testing purposes.
             * Replace it later with MCU sleep command */
            RN2xx3_interface();
            break;

        /* Transmit a unconfirmed LoRa packet */
        case TX_UNCNF:
            numTx++;
            LED_SetHigh();
            getSensorData(&data);
            printSensorData(&data);
            formatPayload(payload, &data);
            if(!RN2xx3_tx_uncnf(payload)) {
#ifdef DEBUGGING
                printf("Tx unsuccessful, entering sleep.\r\n");
#endif
            }           
            /* Put RN2xx3 to sleep, make sure the interval (in ms) is less than
             * the Tx interval (TX_INTERVAL_SEC) which is in seconds.
             * When RN2xx3  wakes up, either an 'ok' will appear in the rxBuffer
             * or 'invalid_param' when improperly formatted or too long */
            RN2xx3_cmd("sys sleep 90000\r\n");
            
            /* Start the PIT in preparation for MCU sleep */
            RTC_Start();
            state = SLEEP;
            LED_SetHigh();
            break;

        case TEST_MOISTURE:
#ifdef DEBUGGING
            printf("Pressing the button will print an ADC value.\n");
            printf("\nSample in dry air first, then sample in a cup of water.\n\n");
#endif
            state = WAIT_FOR_TEST;
            
            break;
            
        case TEST_LORA:
            RN2xx3_interface();
            break;

        case TEST_BME280:
            /* Initialize Weather Click */
            BME280_init();
            
            /* Gather and format sensor data for transmission*/
            getSensorData(&data);
            printSensorData(&data);
            formatPayload(payload, &data);
            state = WAIT_FOR_TEST;
            break;

        case TEST_LOW_POWER:
            state = WAIT_FOR_TEST;
            break;
            
        case TEST_SENSOR_VALUES:
            getSensorData(&data);
            printSensorData(&data);
            state = WAIT_FOR_TEST;
            break;

        case WAIT_FOR_TEST:
            break;
            
        default:
            break;
    }

}

