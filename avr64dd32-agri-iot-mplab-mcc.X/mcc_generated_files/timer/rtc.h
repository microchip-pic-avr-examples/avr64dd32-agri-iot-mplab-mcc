/**
 * RTC Generated Driver API Header File
 *
 * @file rtc.h
 *
 * defgroup rtc RTC
 *
 * @brief This header file provides APIs for the RTC driver.
 *
 * @version RTC Driver Version 2.0.2
*/
/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/


#ifndef RTCDRIVER_H
#define RTCDRIVER_H

#include "../system/utils/compiler.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup rtc
 * @typedef void RTC_cb_t
 * @brief Function pointer to callback function called by RTC. NULL=default value: No callback function is to be used.
 */ 
typedef void (*RTC_cb_t)(void);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if overflow interrupt flag is set.
 * @param RTC_cb_t cb - call back value for overflow.
 * @return none
 */ 
void RTC_SetOVFIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if Compare match interrupt flag is set.
 * @param RTC_cb_t cb - call back value for compare.
 * @return none
 */ 
void RTC_SetCMPIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Isr callback function to be called if PIT interrupt flag is set.
 * @param RTC_cb_t cb - call back value for PIT.
 * @return none
 */ 
void RTC_SetPITIsrCallback(RTC_cb_t cb);
/**
 * @ingroup rtc
 * @brief Initialize RTC interface.
 * @param none
 * @retval 0 - the RTC init was successful
 * @retval 1 - the RTC init was not successful
 */ 
int8_t RTC_Initialize(void);
/**
 * @ingroup rtc
 * @brief API to start the counter register for RTC interface
 * @param none
 * @return none
 */
void RTC_Start(void);
/**
 * @ingroup rtc
 * @brief API to stop the counter register for RTC interface
 * @param none
 * @return none
 */
void RTC_Stop(void);
/**
 * @ingroup rtc
 * @brief API to write the counter value for RTC.
 * @param uint16_t timerVal - Loading the counter value to write for RTC.
 * @return none
 */
void RTC_WriteCounter(uint16_t timerVal);
/**
 * @ingroup rtc
 * @brief API to write the counter value to load for RTC.
 * @param uint16_t timerVal - Loading the write period to determine overflow period in RTC.
 * @return none
 */
void RTC_WritePeriod(uint16_t timerVal);
/**
 * @ingroup rtc
 * @brief API to read the counter clock cycle value from counter register.
 * @param none
 * @return uint16_t - Counter values returns from the RTC interface.
 */
uint16_t RTC_ReadCounter(void);
/**
 * @ingroup rtc
 * @brief API to read the overflow value in period register.
 * @param none
 * @return uint16_t - Period values returns from the RTC interface.
 */
uint16_t RTC_ReadPeriod(void);
/**
 * @ingroup rtc
 * @brief Enable the CMP interrupt to set the flag, if match value between counter register and compare register.
 * @param none
 * @return none
 */
void RTC_EnableCMPInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the CMP interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisableCMPInterrupt(void);
/**
 * @ingroup rtc
 * @brief Enable the overflow interrupt set the OVF flag, if the counter reached value from the period register and wrapped to zero.
 * @param none
 * @return none
 */
void RTC_EnableOVFInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the Overflow interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisableOVFInterrupt(void);
/**
 * @ingroup rtc
 * @brief Enable the Period Interrupt Timer to set the flag, if a time period has passed as configured in period bit field.
 * @param none
 * @return none
 */
void RTC_EnablePITInterrupt(void);
/**
 * @ingroup rtc
 * @brief Disable the PIT interrupt for RTC interface.
 * @param none
 * @return none
 */
void RTC_DisablePITInterrupt(void);
/**
 * @ingroup rtc
 * @brief Clearing the Overflow interrupt flag after the overflow flag set.
 * @param none
 * @return none
 */
void RTC_ClearOVFInterruptFlag(void);
/**
 * @ingroup rtc
 * @brief Enable the Overflow interrupt to set overflow flag, when overflow occur.
 * @param none
 * @return none
 */
bool RTC_IsOVFInterruptEnabled(void);


#endif /* RTCDRIVER_H */

/** @}*/