/* 
 * File:   RN2xx3.h
 * Author: C70348
 *
 * Created on January 15, 2023, 9:41 PM
 */

#ifndef RN2xx3_H
#define	RN2xx3_H

#include "ringBuffer.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
/* Enable print statements for Rx and Tx
 * TODO: add if def's around send to terminal cmds */
//#define DEBUG  

/* Enable one activation method by commenting out the other */
//#define ABP    // Activation By Personalization
#define OTAA   // Over The Air Activation
    
/* Device EUI and Address */
#define HWEUI  "0000000000000000"    
#define DEVEUI "0000000000000000"
#define DEVADDR "00000000"
    
/* ABP Session Keys */
#define NWKSKEY "00000000000000000000000000000000"
#define APPSKEY "00000000000000000000000000000000"
   
/* OTAA Session Keys */
#define APPEUI "0000000000000000"
#define APPKEY "00000000000000000000000000000000"
    
/* TTN Settings */
#define RX1_DELAY "5000"   

/* RN2xx3 Command and Response Strings Maximum Size */
#define RN2xx3_BUFFER_SIZE            255
    
/* Port Designations */
#define PORT_CNF    "10 "
#define PORT_UNCNF  "11 "
    
/* TTN Transmit Interval */
#define TX_INTERVAL_SEC 180    
    
typedef enum {
    RN2903,
    RN2483
} RN2xx3_t;    
    
typedef enum  {
  SINGLE_CHANNEL_EU,
  TTN_EU,
  TTN_US,
  DEFAULT_EU
} FREQ_PLAN_t;

    void loadCharacterToBuffer(char c);    
    void RN2xx3_resp(void);
    void RN2xx3_resp2(void);
    void RN2xx3_cmd(const char *str);
    void RN2xx3_mac_pause(void);
    void RN2xx3_getHWEUI();
    void RN2xx3_sys_RESET();
    bool RN2xx3_mac_reset();
    void RN2xx3_mac_get_status();
    void RN2xx3_init(void);
    void RN2xx3_save(void);
    void RN2xx3_set_freq_plan();
    void RN2xx3_config_OTAA(void);
    void RN2xx3_config_ABP(void);
    void RN2xx3_config_TTN(void);
    bool RN2xx3_join_OTAA(void);
    bool RN2xx3_join_ABP(void);
    bool RN2xx3_join_TTN(void);
    bool RN2xx3_tx_cnf(const char *str);
    bool RN2xx3_tx_uncnf(const char *str);
    void RN2xx3_get_status();
    void RN2xx3_interface();

#ifdef	__cplusplus
}
#endif

#endif	/* RN2xx3_H */

