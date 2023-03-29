/*
? [2022] Microchip Technology Inc. and its subsidiaries.

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

#ifndef RINGBUFFER_H
#define	RINGBUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
    
    //Change the type of this variable to support bigger buffers
    //However, this will significantly increase memory overhead
    typedef uint8_t RingBufferSize;
    
    //Fields Required to Manage a Ring Buffer
    typedef struct {
        char* memory;
        RingBufferSize memSize;
        RingBufferSize readIndex;
        RingBufferSize writeIndex;
    } RingBuffer;
    
    //Creates a ringBuffer from a pool of memory
    void ringBuffer_createBuffer(RingBuffer* buffer, char* memPtr, RingBufferSize size);
    
    //Copies the pointers and variables of SRC to DST
    //DOES NOT COPY the MEMORY CONTENTS
    void ringBuffer_softCopy(RingBuffer* src, RingBuffer* dst);
    
    //Copies the memory contents AND the variables of SRC to DST
    //Must have initialized MEMSIZE and MEMORY
    void ringBuffer_duplicate(RingBuffer* src, RingBuffer* dst);
    
    //Clears the contents of the buffer with '\0' and resets indexes
    void ringBuffer_reset(RingBuffer* buffer);
    
    //Returns true if the buffer is empty
    bool ringBuffer_isEmpty(RingBuffer* buffer);
    
    //Advance the readIndex by 1 position
    //If readIndex exceeds writeIndex, readIndex is set to writeIndex
    void ringBuffer_incrementReadIndex(RingBuffer* buffer);
    
    //Advance the readIndex by LEN positions
    //If readIndex exceeds writeIndex, readIndex is set to writeIndex
    void ringBuffer_advanceReadIndex(RingBuffer* buffer, RingBufferSize len);
    
    //Advances the ringIndex to the END of the 1st instance of STR in the buffer
    //If the string could not be found, this function returns false
    bool ringBuffer_advanceToString(RingBuffer* buffer, const char* str);
    
    //Searches for STR in the buffer
    //Returns true if found, false if not.
    bool ringBuffer_find(RingBuffer* buffer, const char* str);
    
    //Counts the number of characters matching SEARCH
    RingBufferSize ringBuffer_count(RingBuffer* buffer, char search);
    
    //Returns the number of characters that can be read
    RingBufferSize ringBuffer_charsToRead(RingBuffer* buffer);
    
    //Clears the buffer by advancing readIndex to writeIndex
    void ringBuffer_flushReadBuffer(RingBuffer* buffer);
    
    //Advance the writeIndex by 1 Position
    //This function is called by the LOAD functions
    void ringBuffer_incrementWriteIndex(RingBuffer* buffer);
        
    //Loads a character into a ring buffer
    //Returns FALSE if the ringBuffer overflows (operation will still complete, however)
    bool ringBuffer_loadCharacter(RingBuffer* buffer, char input);
    
    //Loads LEN characters into a ring buffer
    //Returns FALSE if the ringBuffer overflows (operation will still complete, however)
    bool ringBuffer_loadCharacters(RingBuffer* buffer, const char* input, RingBufferSize len);
    
    //Loads a NULL-terminated string into the ring buffer
    //Returns FALSE if the ringBuffer overflows (operation will still complete, however)
    bool ringBuffer_loadString(RingBuffer* buffer, const char* input);
    
    //Returns the char at the current readIndex AND advances readIndex
    //If no data is in the buffer, then '\0' is returned
    char ringBuffer_getChar(RingBuffer* buffer);
    
    //Returns the char at the current readIndex but does NOT advance readIndex
    //If no data is in the buffer, then '\0' is returned
    char ringBuffer_peekChar(RingBuffer* buffer);
    
    /*
     * Copies the current ringBuffer to a destination string of LEN bytes. 
     * A NULL terminator will be appended to the dest. string.
     * 
     * Returns the number of bytes (incl. terminator) written.
     * 
     * readIndex is not incremented.
     */
    RingBufferSize ringBuffer_copyBuffer(RingBuffer* buffer, char* dest, RingBufferSize len);
    
    /*
     * Copies the current ringBuffer to a destination string of LEN bytes. 
     * A NULL terminator will NOT be appended to the dest. string.
     * 
     * Returns the number of bytes written.
     * 
     * readIndex is not incremented.
     */
    RingBufferSize ringBuffer_copyRawBuffer(RingBuffer* buffer, char* dest, RingBufferSize len);
    
    /*
     * Copies the current ringBuffer until DELIM or LEN bytes have been copied. 
     * A NULL terminator will be appended to the dest. string.
     * 
     * Returns the number of bytes (incl. terminator) written.
     * 
     * readIndex is not incremented.
     */
    RingBufferSize ringBuffer_copyBufferUntil(RingBuffer* buffer, char* dest, char delim, RingBufferSize len);
    
    /*
     * Copies the current ringBuffer between START and STOP, or until LEN bytes have been copied. 
     * A NULL terminator will be appended to the dest. string.
     * 
     * Returns the number of bytes (incl. terminator) written.
     * (If START never occurs, returns size = 1 from the terminating character)
     * 
     * readIndex is not incremented.
     */
    RingBufferSize ringBuffer_copyAndChop(RingBuffer* buffer, char* dest, char start, char stop, RingBufferSize len);


#ifdef	__cplusplus
}
#endif

#endif	/* RINGBUFFER_H */
