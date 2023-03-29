#include "usart1_utilities.h"

//Sends a constant string to the USART Driver
void USART1_sendString(const char* str)
{
    uint8_t index = 0;
    while (str[index] != '\0')
    {
        if (RN2xx3_TX_IS_READY())
        {
            RN2xx3_TX_SEND(str[index]);
            index++;
        }
    }
}
