#include "mcc_generated_files/system/system.h"
#include "RN2xx3.h"
#include "ringBuffer.h"
#include "usart1.h"
#include "usart1_utilities.h"
#include <util/delay.h>

/* Comment out to suppress debug print statements and conserve power */
#define DEBUGGING

//Size of Ring Buffer
#define RN2xx3_BUFFER_SIZE 255
#define HWEUI_SIZE 17
#define VERSION_SIZE 34
#define STATUS_SIZE 17

//Memory for Ring Buffer
static char cmdBuffer[RN2xx3_BUFFER_SIZE];
static char rcvBuffer[RN2xx3_BUFFER_SIZE];

/* Ring Buffer Structures */
static RingBuffer txBuffer;
static RingBuffer rxBuffer;

/* Module Type */
RN2xx3_t modType;

/* Static Character Buffers  */
static char hweui[HWEUI_SIZE];
static char status[STATUS_SIZE];

/* Function to Load Data Into Ring Buffer */
void loadCharacterToBuffer(char c)
{
    ringBuffer_loadCharacter(&rxBuffer, c);
}

void RN2xx3_cmd(const char *str)
{
    USART1_sendString(str);
    _delay_ms(100);
#ifdef DEBUGGING
    printf("%s\r\n", str);
#endif
}

void RN2xx3_getHWEUI() 
{
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
    //ringBuffer_flushReadBuffer(&rxBuffer);
    RN2xx3_cmd("sys get hweui\r\n");
    _delay_ms(200);
    ringBuffer_copyBuffer(&rxBuffer, &hweui[0], HWEUI_SIZE);
    printf("      HWEUI: %s\r\n", hweui);
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
}

void RN2xx3_sys_RESET()
{
    ringBuffer_flushReadBuffer(&rxBuffer);
    RN2xx3_cmd("sys factoryRESET\r\n");
    _delay_ms(5000);
}

bool RN2xx3_mac_reset()
{
    bool response = false;
    bool reset_ok = false;
    RN2xx3_cmd("mac reset\r\n");
    _delay_ms(50);
    
        /* Receive first response*/
    while(!response)
    {
        /* Look for the \r deliminator, then respond to different cases */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "ok"))
            {
                reset_ok = true;
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_param"))
            {
                response = true;
            }         
        }
    }
    
    //Advance the buffer to the newline character
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
    
    return reset_ok;
}

void RN2xx3_mac_get_status()
{
    ringBuffer_flushReadBuffer(&rxBuffer);
    RN2xx3_cmd("mac get status\r\n");
    ringBuffer_copyBuffer(&rxBuffer, &status[0], STATUS_SIZE);
    printf("Mac status: %s\r\n", status);
    _delay_ms(500);
}

void RN2xx3_init()
{
    printf("\nInitializing RN2xx3...\r\n");
    uint8_t done = 0;
    
    //Initialize Ring Buffers
    ringBuffer_createBuffer(&txBuffer, &cmdBuffer[0], RN2xx3_BUFFER_SIZE);
    ringBuffer_createBuffer(&rxBuffer, &rcvBuffer[0], RN2xx3_BUFFER_SIZE);
        
    //Reset the RN2xx3
    RN2xx3_RESET_SetLow();
    _delay_ms(300);
    RN2xx3_RESET_SetHigh();
    _delay_ms(100);         
    RN2xx3_RTS_SetHigh();
    _delay_ms(500);

    while(!done)
    {
        //Look for the Deliminator
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "RN2903 1.0.3"))
            {
#ifdef DEBUGGING
                printf("\r\nModule Type: RN2903\r\n");
                printf("   Observes: LoRaWAN spec 1.0.0\r\n");
#endif
                modType = RN2903; 
                done = 1;
            }
            else if (ringBuffer_find(&rxBuffer, "RN2483 1.0.5"))
            {
#ifdef DEBUGGING
                printf("\r\nModule Type: RN2483\r\n");
                printf("   Observes: LoRaWAN spec 1.0.2\r\n");
#endif
                modType = RN2483;
                done = 1;
            }
        }
    }
    //Advance the buffer to the newline character
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
}

void RN2xx3_save(void)
{
    RN2xx3_cmd("mac save\r\n");
    _delay_ms(1000);
}

/* The RN2xx3 firmware is designed to match the LoRaWAN specification. The 
 * specification describes a 64(+8) channel plan, making the best use of the 
 * available spectrum in the 915 MHz band. However, the reality is that 64(+8) 
 * channel gateways are still expensive. Most trials and prototypes are using 
 * eight-channel gateways, which are cheaper and widely available. If you are 
 * using the RN2903 with default settings (all channels on) and connecting to an
 *  eight-channel gateway, then seven out of eight packets will fall on a 
 * channel that the gateway is not listening to. Seven packets will be dropped 
 * and only one will pass through. This can be easily corrected by using the 
 * mac set ch status in a loop to disable all the unused channels*/
void RN2xx3_set_freq_plan() {
    char str[32];
    
    switch(modType)
    {
        case RN2483:
            /* Set Rx2 Window */
            RN2xx3_cmd("mac set rx2 3 869525000");
            
            /* Channel 0 */
            RN2xx3_cmd("mac set ch dcycle 0 799");
            
            /* Channel 1 */
            RN2xx3_cmd("mac set ch drrange 1 0 6");
            RN2xx3_cmd("mac set ch dcycle 1 799");
            
            /* Channel 2 */
            RN2xx3_cmd("mac set ch dcycle 2 799");
            
            /* Channel 3 */
            RN2xx3_cmd("mac set ch freq 3 867100000");
            RN2xx3_cmd("mac set ch drrange 3 0 5");
            RN2xx3_cmd("mac set ch dcycle 3 799");
            RN2xx3_cmd("mac set ch status 3 on");
            
            /* Channel 4 */
            RN2xx3_cmd("mac set ch freq 4 867300000");
            RN2xx3_cmd("mac set ch drrange 4 0 5");
            RN2xx3_cmd("mac set ch dcycle 4 799");
            RN2xx3_cmd("mac set ch status 4 on");
            
            /* Channel 5 */
            RN2xx3_cmd("mac set ch freq 5 867500000");
            RN2xx3_cmd("mac set ch drrange 5 0 5");
            RN2xx3_cmd("mac set ch dcycle 5 799");
            RN2xx3_cmd("mac set ch status 5 on");
            
            /* Channel 6 */
            RN2xx3_cmd("mac set ch freq 6 867700000");
            RN2xx3_cmd("mac set ch drrange 6 0 5");
            RN2xx3_cmd("mac set ch dcycle 6 799");
            RN2xx3_cmd("mac set ch status 6 on");
            
            /* Channel 7 */
            RN2xx3_cmd("mac set ch freq 7 867900000");
            RN2xx3_cmd("mac set ch drrange 7 0 5");
            RN2xx3_cmd("mac set ch dcycle 7 799");
            RN2xx3_cmd("mac set ch status 7 on");
            
            break;
            
        case RN2903:
            for(uint8_t i = 0; i < 72; i++) {
                if(i >= 8 && i < 16 )  {
                    sprintf(str,"mac set ch status %d on\r\n", i);
                    RN2xx3_cmd(str);
                }
                else    {
                    sprintf(str,"mac set ch status %d off\r\n", i);
                    RN2xx3_cmd(str);
                }
            };
            
            break;
            
        default:
            printf("Module not specified\n");
    }

}

void RN2xx3_config_ABP() 
{
    /* Add a mac reset if you plan to power cycle between joins, remove if not 
     * additionally you may need to reset the mac on the TTN side */
    if(!RN2xx3_mac_reset()) {
        RN2xx3_mac_reset();
    }
#ifdef DEBUGGING
    printf("\n\nConfiguring RN2xx3 For ABP join (expect ~10 sec to set channel frequency plan)...\n");
#endif
    RN2xx3_cmd("mac set deveui " DEVEUI "\r\n");
    RN2xx3_cmd("mac set devaddr " DEVADDR "\r\n");
    RN2xx3_cmd("mac set appskey " APPSKEY "\r\n");
    RN2xx3_cmd("mac set nwkskey " NWKSKEY "\r\n");
    RN2xx3_cmd("mac set ar on\r\n");
    RN2xx3_cmd("mac set rxdelay1 " RX1_DELAY "\r\n");
    RN2xx3_cmd("mac set rx2 11 923300000\r\n");
    RN2xx3_cmd("mac set dr 2\r\n");
    RN2xx3_cmd("mac set adr on\r\n");
    RN2xx3_set_freq_plan();
    RN2xx3_save();    
}
/* Only DEVEUI, APPKEY, and APPEUI are necessary for OTAA, but DEVADDR, APPSKEY,
 * and NWKSKEY are cleared.  When registering devices on TTN, the HWEUI was used
 * as DEVEUI for OTAA.  
 */
void RN2xx3_config_OTAA()
{
    /* Add a mac reset if you plan to power cycle between joins, remove if not 
     * additionally you may need to reset the mac on the TTN side */
    if(!RN2xx3_mac_reset()) {
        RN2xx3_mac_reset();
    }
#ifdef DEBUGGING    
    printf("\r\nConfiguring for OTAA join (expect ~10 sec to set channel frequency plan)...\n");
#endif
    RN2xx3_cmd("mac pause\r\n");
    RN2xx3_cmd("mac resume\r\n");  
    RN2xx3_cmd("mac set deveui " HWEUI "\r\n");
    RN2xx3_cmd("mac set devaddr 00000000\r\n");
    RN2xx3_cmd("mac set appskey 00000000000000000000000000000000\r\n");
    RN2xx3_cmd("mac set nwkskey 00000000000000000000000000000000\r\n");
    RN2xx3_cmd("mac set appkey " APPKEY "\r\n");
    RN2xx3_cmd("mac set appeui " APPEUI "\r\n");
    RN2xx3_cmd("mac set ar on\r\n");
    RN2xx3_cmd("mac set rxdelay1 " RX1_DELAY "\r\n");
    
    /* Needed to change to 13 (SF7/500 kHz) when ~50 m away, when gateway is
     * over 200m use default 8 (SF12/ 500 kHz otherwise strength of accept 
     * message causes bleed into other channels and RN2xx3 will ignore and
     * output denied on Rx */
    RN2xx3_cmd("mac set rx2 13 923300000\r\n");
    RN2xx3_cmd("mac set dr 3\r\n");
    RN2xx3_cmd("mac set adr on\r\n");
    
    RN2xx3_set_freq_plan();
    RN2xx3_save(); 
}

void RN2xx3_config_TTN(void)
{   
#ifdef OTAA
    RN2xx3_config_OTAA();
#endif
    
#ifdef ABP
    RN2xx3_config_ABP();
#endif
}

bool RN2xx3_join_OTAA() 
{
    bool response = false;
    bool accepted = false;
    ringBuffer_flushReadBuffer(&rxBuffer);
#ifdef DEBUGGING
    printf("\nAttempting to join TTN over OTAA...\n");
#endif
    
    RN2xx3_cmd("mac join otaa\r\n");
    _delay_ms(300);
    
    /* Receive first response*/
    while(!response)
    {
        /* Look for the Deliminator */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "ok"))
            {
                ringBuffer_flushReadBuffer(&rxBuffer);
#ifdef DEBUGGING
                printf("\r\nSuccessfully sent join request...\r\n"); 
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "keys_not_init"))
            {
#ifdef DEBIGGING
                printf("\r\nIncorrect keys, check again and reprogram\r\n");
#endif
                response = true;
            }
        }
    }
    
    //Advance the buffer to the newline character
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
    
    _delay_ms(300);
    
    /* Receive 2nd response */
    response = false;
    while(!response)
    {
        /* Look for the Deliminator */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "accepted"))
            {
                //Advance the buffer to the newline character
                ringBuffer_flushReadBuffer(&rxBuffer);
#ifdef DEBUGGING
                printf("\r\nJoin accept message received...\r\nRx: accepted\r\n");
#endif
                accepted = true;
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "denied"))
            {
                //Advance the buffer to the newline character
                ringBuffer_flushReadBuffer(&rxBuffer);
#ifdef DEBUGGING
                printf("\r\nNode did not receive an accept message.\r\n");
                printf("\r\nNode has not joined. The gateway may be ignoring your request (MIC fail).\r\n");
                printf("The application may be expecting a different APPKEY or your device is too close to the gateway\r\n");
                printf("Move your device to at least ~10m away and if possible check the gateway log to verify.\r\n");
                printf("It may also try setting a different datarate, if the default dr 0 was used try dr 1.\r\n");
#endif
                accepted = false;
                response = true;
            }
        }
    }
    if(accepted) {
        RN2xx3_save();
    }
    return accepted;
}

/* During and ABP join request, the RN2xx3 does not send an request message.
 * Receiving an accept response to an mac join abp, means that keys have been
 * configured correctly. */
bool RN2xx3_join_ABP()
{
    bool response = false;
    bool accepted = false;
#ifdef DEBUGGING
    printf("\nAttempting to join over ABP...\n");
#endif
    
    ringBuffer_flushReadBuffer(&rxBuffer);
    
    RN2xx3_cmd("mac join abp\r\n");
    
    /* Receive response from RN2xx3 */
    while(!response)
     {
        /* Look for the Deliminator */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "accepted"))
            {
                //Advance the buffer to the newline character
                ringBuffer_flushReadBuffer(&rxBuffer);
#ifdef DEBUGGING
                printf("\r\nRx: accepted\r\n");
                printf("Device is joined over ABP.\r\n");
#endif
                accepted = true;
                response = true;
            }
            
            else if (ringBuffer_find(&rxBuffer, "keys_not_init"))
            {
#ifdef DEBUGGING
                printf("\r\nIncorrect keys, check again and reprogram\r\n");
#endif
                response = true;
            }
        }
    }
    _delay_ms(300);

    if(accepted) {
        RN2xx3_save();
    }
    
    /* Clear the receive buffer */
    ringBuffer_flushReadBuffer(&rxBuffer);

    return accepted;
}  

bool RN2xx3_join_TTN()
{
#ifdef OTAA
    return RN2xx3_join_OTAA();
#endif
    
#ifdef ABP
    return RN2xx3_join_ABP();
#endif
}

bool RN2xx3_tx_cnf(const char *str)
{
    bool response = false;
    bool tx_ok = false;
    
    /* Clear the receive buffer */
    ringBuffer_flushReadBuffer(&rxBuffer);
    
    /* For debug purposes, prints the command on the terminal */
#ifdef DEBUGGING
    printf("\n\nTransmitting Data\n");
    printf("----------------------------------------------------\n");
    printf("\nTx: mac tx cnf " PORT_CNF);
    printf(str);
#endif
    _delay_ms(120);

    /* Concatenate the unconfirmed tx string  */
    ringBuffer_loadCharacters(&txBuffer, "mac tx cnf ", 11);
    ringBuffer_loadCharacters(&txBuffer, PORT_CNF , 3);
    ringBuffer_loadCharacters(&txBuffer, str, 10);
    
    /* Transmit the buffered message over the RN2xx3 UART */
    while(!ringBuffer_isEmpty(&txBuffer))
    {
        if(RN2xx3_TX_IS_READY())
        {
            RN2xx3_TX_SEND(ringBuffer_getChar(&txBuffer));
        }
    }
    _delay_ms(10);
    
    /* Receive first response*/
    while(!response)
    {
        /* Look for the \r deliminator, then respond to different cases */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "ok"))
            {
#ifdef DEBUGGING
                printf("\r\nParameters and configurations are valid. \r\nThe "
                        "packet was forwarded to the radio for Tx.\r\n"); 
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_param"))
            {
#ifdef DEBUGGING
                printf("\r\nPort, data, and/or type(cnf or uncnf) not valid.\r\n");
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "not_joined"))
            {
#ifdef DEBUGGING
                printf("\r\nThe node is not joined with the network.\r\n"
                        "Payload not sent.\r\n");
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_data_len"))
            {
#ifdef DEBUGGING
                printf("\r\nThe payload is too large for the data rate.\r\n");
#endif
                response = true;
                return false;
            }          
        }
    }
    
    //Advance the buffer to the newline character
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
    
    _delay_ms(100);
    
    /* Receive 2nd response */
    response = false;
    while(!response)
    {
        /* Look for the Deliminator */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "mac_tx_ok"))
            {
#ifdef DEBUGGING
                printf("\r\nRadio successfully transmitted the confirmed uplink.\r\n");
#endif
                response = true;
                tx_ok = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_data_len"))
            {
#ifdef DEBUGGING
                printf("\r\nThe payload is too large for the data rate.\r\n");
                printf("This can occur if back-off (retransmissions) changed the datarate.\r\n");
#endif
                response = true;
                return false;
            }
            else if (ringBuffer_find(&rxBuffer, "mac_err"))
            {
#ifdef DEBUGGING
                printf("\r\nAn ACK was not received back from the server.\r\n");
#endif
                response = true;
            }
        }
    }
    return tx_ok;
}

bool RN2xx3_tx_uncnf(const char *str)
{
    bool response = false;
    
    /* Clear the receive buffer */
    ringBuffer_flushReadBuffer(&rxBuffer);
    
    /* For debug purposes, prints the command on the terminal */
#ifdef DEBUGGING
    printf("\n\nTransmitting Data\n");
    printf("----------------------------------------------------\n");
    printf("\nTx: mac tx uncnf " PORT_UNCNF);
    printf(str);
#endif
    _delay_ms(120);

    /* Concatenate the unconfirmed tx string  */
    ringBuffer_loadCharacters(&txBuffer, "mac tx uncnf ", 13);
    ringBuffer_loadCharacters(&txBuffer, PORT_UNCNF , 3);
    ringBuffer_loadCharacters(&txBuffer, str, 10);
    
    /* Transmit the buffered message over the RN2xx3 UART */
    while(!ringBuffer_isEmpty(&txBuffer))
    {
        if(RN2xx3_TX_IS_READY())
        {
            RN2xx3_TX_SEND(ringBuffer_getChar(&txBuffer));
        }
    }
    _delay_ms(10);
    
    /* Receive first response*/
    while(!response)
    {
        /* Look for the \r deliminator, then respond to different cases */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "ok"))
            {
#ifdef DEBUGGING
                printf("\r\nParameters and configurations are valid. \r\nThe "
                        "packet was forwarded to the radio for Tx.\r\n"); 
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_param"))
            {
#ifdef DEBUGGING
                printf("\r\nPort, data, and/or type(cnf or uncnf) not valid.\r\n");
#endif
                response = true;
                return false;
            }
            else if (ringBuffer_find(&rxBuffer, "not_joined"))
            {
#ifdef DEBUGGING
                printf("\r\nThe node is not joined with the network.\r\n"
                        "Payload not sent.\r\n");
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_data_len"))
            {
#ifdef DEBUGGING
                printf("\r\nThe payload is too large for the data rate.\r\n");
#endif
                response = true;
                return false;
            }          
        }
    }
    
    _delay_ms(100);
    
    //Advance the buffer to the newline character
    ringBuffer_advanceToString(&rxBuffer, "\r\n");
    
    /* Receive 2nd response */
    response = false;
    while(!response)
    {
        /* Look for the Deliminator */
        if (ringBuffer_find(&rxBuffer, "\r"))
        {
            if (ringBuffer_find(&rxBuffer, "mac_tx_ok"))
            {
#ifdef DEBUGGING
                printf("\r\nRadio successfully transmitted the unconfirmed uplink.\r\n");
#endif
                response = true;
            }
            else if (ringBuffer_find(&rxBuffer, "invalid_data_len"))
            {
#ifdef DEBUGGING
                printf("\r\nThe payload is too large for the data rate.\r"
                        "This can occur if back-off (retransmissions) changed the dr.\r\n");
#endif
                response = true;
                return false;
            }
            else if (ringBuffer_find(&rxBuffer, "mac_err"))
            {
#ifdef DEBUGGING
                printf("\r\nAN ACK was not received back from the server.\r\n");
#endif
                response = true;
            }
        }
    }
    return true;
}

void RN2xx3_interface() 
{
        if(UART0__IsRxReady()) {
            ringBuffer_loadCharacter(&txBuffer, UART0_Read());
        }    
        if(!ringBuffer_isEmpty(&rxBuffer) && UART0_IsTxReady()) {
            UART0_Write(ringBuffer_getChar(&rxBuffer));
        }
        if(!ringBuffer_isEmpty(&txBuffer) && RN2xx3_TX_IS_READY()) {
            RN2xx3_TX_SEND(ringBuffer_getChar(&txBuffer));
        }  
}