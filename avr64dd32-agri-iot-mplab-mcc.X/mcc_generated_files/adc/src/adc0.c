/**
 * ADC0 Generated Driver File
 *
 * @file adc0.c
 * 
 * @ingroup adc0 
 * 
 * @brief This file contains the driver code for ADC0 module.
 * 
 * @version ADC0 Driver Version 1.0.0
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


#include "../adc0.h"

adc_irq_cb_t ADC0_window_cb = NULL;
adc_irq_cb_t ADC0_resrdy_cb = NULL;

int8_t ADC0_Initialize(void)
{
     
    // SAMPNUM No accumulation; 
    ADC0.CTRLB = 0x0;

    // PRESC CLK_PER divided by 4; 
    ADC0.CTRLC = 0x1;

    // INITDLY DLY0; SAMPDLY DLY0; 
    ADC0.CTRLD = 0x0;

    // WINCM No Window Comparison; 
    ADC0.CTRLE = 0x0;

    // DBGRUN disabled; 
    ADC0.DBGCTRL = 0x0;

    // STARTEI disabled; 
    ADC0.EVCTRL = 0x0;

    // RESRDY disabled; WCMP disabled; 
    ADC0.INTCTRL = 0x0;

    // MUXPOS ADC input pin 1; 
    ADC0.MUXPOS = 0x1;

    // MUXNEG Ground; 
    ADC0.MUXNEG = 0x40;

    // SAMPLEN 255; 
    ADC0.SAMPCTRL = 0xFF;

    // Window comparator high threshold 
    ADC0.WINHT = 0xFFF;

    // Window comparator low threshold 
    ADC0.WINLT = 0x0;

    // ENABLE enabled; FREERUN enabled; RESSEL 12-bit mode; RUNSTBY disabled; CONVMODE disabled; LEFTADJ disabled; 
    ADC0.CTRLA = 0x3;


    return 0;
}

void ADC0_Enable(void)
{
    ADC0.CTRLA |= ADC_ENABLE_bm;
}

void ADC0_Disable(void)
{
    ADC0.CTRLA &= ~ADC_ENABLE_bm;
}

void ADC0_EnableAutoTrigger(void)
{
    ADC0.EVCTRL |= ADC_STARTEI_bm;
}

void ADC0_DisableAutoTrigger(void)
{
    ADC0.EVCTRL &= ~ADC_STARTEI_bm;
}

void ADC0_SetWindowHigh(adc_result_t high)
{
    ADC0.WINHT = high;
}

void ADC0_SetWindowLow(adc_result_t low)
{
    ADC0.WINLT = low;
}

void ADC0_SetWindowMode(ADC0_window_mode_t mode)
{
    ADC0.CTRLE = mode;
}

void ADC0_SetWindowChannel(adc_0_channel_t channel)
{
    ADC0.MUXPOS = channel;
}

void ADC0_StartConversion(adc_0_channel_t channel)
{
    ADC0.MUXPOS  = channel;
    ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_StartDiffConversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
    ADC0.MUXPOS  = channel;
    ADC0.MUXNEG  = channel1;
    ADC0.COMMAND = ADC_STCONV_bm;
}

void ADC0_StopConversion(void)
{
    ADC0.COMMAND = ADC_SPCONV_bm;
}

bool ADC0_IsConversionDone(void)
{
    return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

adc_result_t ADC0_GetConversionResult(void)
{
    return (ADC0.RES);
}

bool ADC0_GetWindowResult(void)
{
    bool temp     = (ADC0.INTFLAGS & ADC_WCMP_bm);
    ADC0.INTFLAGS = ADC_WCMP_bm; // Clear intflag if set
    return temp;
}

adc_result_t ADC0_GetConversion(adc_0_channel_t channel)
{
    adc_result_t res;

    ADC0_StartConversion(channel);
    while (!ADC0_IsConversionDone());
    res           = ADC0_GetConversionResult();
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return res;
}

 diff_adc_result_t ADC0_GetDiffConversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
    diff_adc_result_t res;

    ADC0_StartDiffConversion(channel, channel1);
    while (!ADC0_IsConversionDone());
    res = ADC0_GetConversionResult();
    ADC0.INTFLAGS |= ADC_RESRDY_bm;
    return res;
}

uint8_t ADC0_GetResolution(void)
{
    return (ADC0.CTRLA & ADC_RESSEL0_bm) ? 10 : 12;
}

void ADC0_RegisterWindowCallback(adc_irq_cb_t f)
{
    ADC0_window_cb = f;
}

void ADC0_RegisterResrdyCallback(adc_irq_cb_t f)
{
    ADC0_resrdy_cb = f;
}

ISR(ADC0_WCMP_vect)
{        
    // Clear the interrupt flag
    ADC0.INTFLAGS = ADC_WCMP_bm;

    if (ADC0_window_cb != NULL)
    {
        ADC0_window_cb();
    }
}

ISR(ADC0_RESRDY_vect)
{
    // Clear the interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    if (ADC0_resrdy_cb != NULL)
    {
        ADC0_resrdy_cb();
    }
}