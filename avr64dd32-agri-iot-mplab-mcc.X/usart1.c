#include "usart1.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*rxHandler)(char);
static volatile bool isRXBusy = false;

void USART1_init(void)
{
    //Init Callback
    rxHandler = 0;
    
    //Enable Run in Debug
    USART1.DBGCTRL = USART_DBGRUN_bm;
    
    //Enable RX Interrupts
    USART1.CTRLA = USART_RXCIE_bm;
    
    //Async Mode, No Parity, 1 Stop Bit, 8 bit TX
    USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
    
    //Baud Rate 57600
    USART1.BAUD = 694;
    
    //Enable Normal Mode for USART
    USART1.CTRLB = USART_RXMODE_NORMAL_gc;
    
    //Note: Call enableTX/enableRX after this function
}

//Enable TX 
void USART1_enableTX(void)
{
    USART1.CTRLB |= USART_TXEN_bm;
}

//Disable TX
void USART1_disableTX(void)
{
    USART1.CTRLB &= ~(USART_TXEN_bm);
}

//Enable RX
void USART1_enableRX(void)
{
    USART1.CTRLB |= USART_RXEN_bm;
}

//Disable RX
void USART1_disableRX(void)
{
    USART1.CTRLB &= ~(USART_RXEN_bm);
}

//Sets the callback function for Data RX
void USART1_setRXCallback(void (*function)(char))
{
    rxHandler = function;
}

//Sends a byte of data
bool USART1_sendByte(char c)
{
    if (USART1_canTransmit())
    {
        //Clear done bit
        USART1.STATUS |= USART_TXCIF_bm;

        //Load Byte of Data
        USART1.TXDATAL = c;
        return true;
    }
    return false;
}

//Returns true if the peripheral can accept a new byte
bool USART1_canTransmit(void)
{
    return (USART1.STATUS & USART_DREIF_bm);
}

//Returns true if the shifter is running
bool USART1_isBusy(void)
{
    return (!(USART1.STATUS & USART_TXCIF_bm));
}

//Returns true if data is being shifted in
bool USART1_isRXActive(void)
{
    return isRXBusy;
}

ISR(USART1_RXC_vect)
{
    //Data    
    char rx = USART1.RXDATAL;
    if (rxHandler)
    {
        rxHandler(rx);
    }
    isRXBusy = false;
}