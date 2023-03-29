#ifndef TWI_H
#define	TWI_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
    
    //Configures the TWI Peripheral in Host Mode
    void TWI_initHost(void);
    
    //Sends a byte to a client
    bool TWI_sendByte(uint8_t addr, uint8_t data);
    
    //Sends len bytes to a client device
    bool TWI_sendBytes(uint8_t addr, uint8_t* data, uint8_t len);

    //Read a byte from a client
    bool TWI_readByte(uint8_t addr, uint8_t* data);
    
    //Read len bytes from a client
    bool TWI_readBytes(uint8_t addr, uint8_t* data, uint8_t len);
    
    //Send the rAddr byte to client, then read len bytes back
    bool TWI_sendAndReadBytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TWI_H */

