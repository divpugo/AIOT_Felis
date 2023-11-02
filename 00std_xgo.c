#include <string.h>
#include "nrf52833.h"
#include <stdio.h>
#include <nrf.h>


/*
sources:
    https://wiki.elecfreaks.com/en/microbit/robot/xgo-robot-kit/
    https://github.com/elecfreaks/pxt-xgo/blob/master/main.ts
    https://makecode.microbit.org/device/pins
    https://wiki.elecfreaks.com/en/pico/cm4-xgo-robot-kit/advanced-development/serial-protocol/
    
    UART
        tx==P2 onthe micro:bit silkscreen==P0.04 on the nRF (RING2 line on schematic)
        rx==P1 onthe micro:bit silkscreen==P0.03 on the nRF (RING1 line on schematic)
        baudrate: 115200
*/

#define ACTION_GET_DOWN                 1
#define ACTION_STAND_UP                 2
#define ACTION_CREEP_FORWARD            3
#define ACTION_CIRCLE_AROUND            4
#define ACTION_SQUAT_UP                 6
#define ACTION_TURN_ROLL                7
#define ACTION_TURN_PITCH               8
#define ACTION_TURN_YAW                 9
#define ACTION_THREE_AXIS_ROTATION      10
#define ACTION_PEE                      11
#define ACTION_SIT_DOWN                 12
#define ACTION_WAVE1                    13
#define ACTION_STRETCH                  14
#define ACTION_WAVE2                    15
#define ACTION_SWING_LEFT_AND_RIGHT     16
#define ACTION_BEGGING_FOR_FOOD         17
#define ACTION_LOOKING_FOR_FOOD         18
#define ACTION_SHAKE_HANDS              19
#define ACTION_CHICKEN_HEAD             20
#define ACTION_PUSH_UPS                 21
#define ACTION_LOOK_AROUND              22
#define ACTION_DANCE                    23
#define ACTION_NAUGHTY                  24
#define ACTION_CATCH_UP                 128
#define ACTION_CAUGHT                   129
#define ACTION_CATCH                    130
#define ACTION_RESTORE DEFAULT POSTURE  255

#define ROW1_DISCONNECT()    (NRF_P0->PIN_CNF[21]     = 0x00000002 )          // P0.21
#define ROW2_DISCONNECT()    (NRF_P0->PIN_CNF[22]     = 0x00000002 )          // P0.22
#define ROW3_DISCONNECT()    (NRF_P0->PIN_CNF[15]     = 0x00000002 )          // P0.15
#define ROW4_DISCONNECT()    (NRF_P0->PIN_CNF[24]     = 0x00000002 )          // P0.24
#define ROW5_DISCONNECT()    (NRF_P0->PIN_CNF[19]     = 0x00000002 )          // P0.19

#define ROW1_OUTPUT()        (NRF_P0->PIN_CNF[21]     = 0x00000003 )          // P0.21
#define ROW2_OUTPUT()        (NRF_P0->PIN_CNF[22]     = 0x00000003 )          // P0.22
#define ROW3_OUTPUT()        (NRF_P0->PIN_CNF[15]     = 0x00000003 )          // P0.15
#define ROW4_OUTPUT()        (NRF_P0->PIN_CNF[24]     = 0x00000003 )          // P0.24
#define ROW5_OUTPUT()        (NRF_P0->PIN_CNF[19]     = 0x00000003 )          // P0.19

#define ROW1_HIGH()          (NRF_P0->OUTSET          = (0x00000001 << 21) )   // P0.21
#define ROW2_HIGH()          (NRF_P0->OUTSET          = (0x00000001 << 22) )   // P0.22
#define ROW3_HIGH()          (NRF_P0->OUTSET          = (0x00000001 << 15) )   // P0.15
#define ROW4_HIGH()          (NRF_P0->OUTSET          = (0x00000001 << 24) )   // P0.24
#define ROW5_HIGH()          (NRF_P0->OUTSET          = (0x00000001 << 19) )   // P0.19
                        
#define COL1_DISCONNECT()    (NRF_P0->PIN_CNF[28]     = 0x00000002 )           // P0.28
#define COL2_DISCONNECT()    (NRF_P0->PIN_CNF[11]     = 0x00000002 )           // P0.11
#define COL3_DISCONNECT()    (NRF_P0->PIN_CNF[31]     = 0x00000002 )           // P0.31
#define COL4_DISCONNECT()    (NRF_P1->PIN_CNF[05]     = 0x00000002 )           // P1.05
#define COL5_DISCONNECT()    (NRF_P0->PIN_CNF[30]     = 0x00000002 )           // P0.30

#define COL1_OUTPUT()        (NRF_P0->PIN_CNF[28]     = 0x00000003 )           // P0.28
#define COL2_OUTPUT()        (NRF_P0->PIN_CNF[11]     = 0x00000003 )           // P0.11
#define COL3_OUTPUT()        (NRF_P0->PIN_CNF[31]     = 0x00000003 )           // P0.31
#define COL4_OUTPUT()        (NRF_P1->PIN_CNF[05]     = 0x00000003 )           // P1.05
#define COL5_OUTPUT()        (NRF_P0->PIN_CNF[30]     = 0x00000003 )           // P0.30
                             
#define COL1_LOW()           (NRF_P0->OUTCLR          = (0x00000001 << 28) )   // P0.28
#define COL2_LOW()           (NRF_P0->OUTCLR          = (0x00000001 << 11) )   // P0.11
#define COL3_LOW()           (NRF_P0->OUTCLR          = (0x00000001 << 31) )   // P0.31
#define COL4_LOW()           (NRF_P1->OUTCLR          = (0x00000001 << 05) )   // P1.05
#define COL5_LOW()           (NRF_P0->OUTCLR          = (0x00000001 << 30) )   // P0.30

typedef enum {
    LED11,LED12,LED13,LED14,LED15,
    LED21,LED22,LED23,LED24,LED25,
    LED31,LED32,LED33,LED34,LED35,
    LED41,LED42,LED43,LED44,LED45,
    LED51,LED52,LED53,LED54,LED55
} led_id_t;

const led_id_t LEDS_SMILEY[] = {
   
          LED22,      LED24,
    
    LED41,                  LED45,
          LED52,LED53,LED54
};

void leds_all_off(void) {
   
    ROW1_DISCONNECT();
    ROW2_DISCONNECT();
    ROW3_DISCONNECT();
    ROW4_DISCONNECT();
    ROW5_DISCONNECT();

    COL1_DISCONNECT();
    COL2_DISCONNECT();
    COL3_DISCONNECT();
    COL4_DISCONNECT();
    COL5_DISCONNECT();
}

void leds_on(led_id_t led_id) {
    
    switch(led_id) {
        
        case LED11: ROW1_OUTPUT(); ROW1_HIGH(); COL1_OUTPUT(); COL1_LOW(); break;
        case LED12: ROW1_OUTPUT(); ROW1_HIGH(); COL2_OUTPUT(); COL2_LOW(); break;
        case LED13: ROW1_OUTPUT(); ROW1_HIGH(); COL3_OUTPUT(); COL3_LOW(); break;
        case LED14: ROW1_OUTPUT(); ROW1_HIGH(); COL4_OUTPUT(); COL4_LOW(); break;
        case LED15: ROW1_OUTPUT(); ROW1_HIGH(); COL5_OUTPUT(); COL5_LOW(); break;

        case LED21: ROW2_OUTPUT(); ROW2_HIGH(); COL1_OUTPUT(); COL1_LOW(); break;
        case LED22: ROW2_OUTPUT(); ROW2_HIGH(); COL2_OUTPUT(); COL2_LOW(); break;
        case LED23: ROW2_OUTPUT(); ROW2_HIGH(); COL3_OUTPUT(); COL3_LOW(); break;
        case LED24: ROW2_OUTPUT(); ROW2_HIGH(); COL4_OUTPUT(); COL4_LOW(); break;
        case LED25: ROW2_OUTPUT(); ROW2_HIGH(); COL5_OUTPUT(); COL5_LOW(); break;

        case LED31: ROW3_OUTPUT(); ROW3_HIGH(); COL1_OUTPUT(); COL1_LOW(); break;
        case LED32: ROW3_OUTPUT(); ROW3_HIGH(); COL2_OUTPUT(); COL2_LOW(); break;
        case LED33: ROW3_OUTPUT(); ROW3_HIGH(); COL3_OUTPUT(); COL3_LOW(); break;
        case LED34: ROW3_OUTPUT(); ROW3_HIGH(); COL4_OUTPUT(); COL4_LOW(); break;
        case LED35: ROW3_OUTPUT(); ROW3_HIGH(); COL5_OUTPUT(); COL5_LOW(); break;

        case LED41: ROW4_OUTPUT(); ROW4_HIGH(); COL1_OUTPUT(); COL1_LOW(); break;
        case LED42: ROW4_OUTPUT(); ROW4_HIGH(); COL2_OUTPUT(); COL2_LOW(); break;
        case LED43: ROW4_OUTPUT(); ROW4_HIGH(); COL3_OUTPUT(); COL3_LOW(); break;
        case LED44: ROW4_OUTPUT(); ROW4_HIGH(); COL4_OUTPUT(); COL4_LOW(); break;
        case LED45: ROW4_OUTPUT(); ROW4_HIGH(); COL5_OUTPUT(); COL5_LOW(); break;

        case LED51: ROW5_OUTPUT(); ROW5_HIGH(); COL1_OUTPUT(); COL1_LOW(); break;
        case LED52: ROW5_OUTPUT(); ROW5_HIGH(); COL2_OUTPUT(); COL2_LOW(); break;
        case LED53: ROW5_OUTPUT(); ROW5_HIGH(); COL3_OUTPUT(); COL3_LOW(); break;
        case LED54: ROW5_OUTPUT(); ROW5_HIGH(); COL4_OUTPUT(); COL4_LOW(); break;
        case LED55: ROW5_OUTPUT(); ROW5_HIGH(); COL5_OUTPUT(); COL5_LOW(); break;
    }
}

static uint8_t pdu[8+1] = { 0 };

//ROBOT

uint8_t BUF_ACTION[] = {
    0x55,            // header 1
    0x00,            // header 2
    0x09,            // length
    0x00,            // command_type
    0x3e,            // address (0x3e==action command)
    12,              // data    (action)
    0x00,            // crc (will be computed below)
    0x00,            // end 1
    0xAA,            // end 2
};

uint8_t uartvars_rxBuf[9];

uint8_t _compute_crc(void) {
    uint16_t tmp;
    tmp += BUF_ACTION[2]; // length
    tmp += BUF_ACTION[3]; // command_type
    tmp += BUF_ACTION[4]; // address
    tmp += BUF_ACTION[5]; // datatmpz
    return 0xff - (uint8_t)(tmp & 0x00ff);
}

int main(void) {
    //Smiley
    uint8_t i;

    leds_on(LED11);

    i=0;
    //RADIO
    // confiureg HF clock
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}

    // configure radio
    NRF_RADIO->MODE          = (  RADIO_MODE_MODE_Ble_LR125Kbit << RADIO_MODE_MODE_Pos);
    NRF_RADIO->TXPOWER       = (  RADIO_TXPOWER_TXPOWER_Pos8dBm << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->PCNF0         = (                              8 << RADIO_PCNF0_LFLEN_Pos)          |
                               (                              1 << RADIO_PCNF0_S0LEN_Pos)          |
                               (                              0 << RADIO_PCNF0_S1LEN_Pos)          |
                               (                              2 << RADIO_PCNF0_CILEN_Pos)          |
                               (     RADIO_PCNF0_PLEN_LongRange << RADIO_PCNF0_PLEN_Pos)           |
                               (                              3 << RADIO_PCNF0_TERMLEN_Pos);
    NRF_RADIO->PCNF1         = (                    sizeof(pdu) << RADIO_PCNF1_MAXLEN_Pos)         |
                               (                              0 << RADIO_PCNF1_STATLEN_Pos)        |
                               (                              3 << RADIO_PCNF1_BALEN_Pos)          |
                               (      RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos)         |
                               (   RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos);
    NRF_RADIO->BASE0         = 0xAAAAAAAAUL;
    NRF_RADIO->TXADDRESS     = 0UL;
    NRF_RADIO->RXADDRESSES   = (RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos);
    NRF_RADIO->TIFS          = 0;
    NRF_RADIO->CRCCNF        = (         RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos)           |
                               (     RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);
    NRF_RADIO->CRCINIT       = 0xFFFFUL;
    NRF_RADIO->CRCPOLY       = 0x00065b; // CRC poly: x^16 + x^12^x^5 + 1
    NRF_RADIO->FREQUENCY     = 30;
    NRF_RADIO->PACKETPTR     = (uint32_t)pdu;

    // receive
    NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
                        (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos) |
                        (RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos);
    NRF_RADIO->TASKS_RXEN    = 1;

    NRF_RADIO->INTENCLR = 0xffffffff;
    NVIC_EnableIRQ(RADIO_IRQn);
    NRF_RADIO->INTENSET = (RADIO_INTENSET_DISABLED_Enabled << RADIO_INTENSET_DISABLED_Pos);

    
    
    //ROBOT
    // start hfclock
    NRF_CLOCK->EVENTS_HFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    // configure P0.04 (UART TX) as output
    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: DIR:     1=Output
    // .... .... .... .... .... .... .... ..B. B: INPUT:   1=Disconnect
    // .... .... .... .... .... .... .... CC.. C: PULL:    0=Disabled
    // .... .... .... .... .... .DDD .... .... D: DRIVE:   0=S0S1
    // .... .... .... ..EE .... .... .... .... E: SENSE:   0=Disabled
    // xxxx xxxx xxxx xx00 xxxx xxxx xxxx 0011 
    //    0    0    0    0    0    0    0    3 0x00000003
    NRF_P0->PIN_CNF[4]                 = 0x00000003;
    NRF_P0->OUTSET                     = (0x00000001 << 4);

    // configure P0.03 (UART RX) as input
    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: DIR:     0=Input
    // .... .... .... .... .... .... .... ..B. B: INPUT:   0=Connect
    // .... .... .... .... .... .... .... CC.. C: PULL:    3=Pullup
    // .... .... .... .... .... .DDD .... .... D: DRIVE:   0=S0S1
    // .... .... .... ..EE .... .... .... .... E: SENSE:   0=Disabled
    // xxxx xxxx xxxx xx00 xxxx xxxx xxxx 1100 
    //    0    0    0    0    0    0    0    c 0x0000000c
    NRF_P0->PIN_CNF[3]                 = 0x0000000c;

    // configure
    NRF_UARTE0->TXD.PTR                = (uint32_t)BUF_ACTION;
    NRF_UARTE0->TXD.MAXCNT             = sizeof(BUF_ACTION);
    NRF_UARTE0->RXD.PTR                = (uint32_t)uartvars_rxBuf;
    NRF_UARTE0->RXD.MAXCNT             = sizeof(uartvars_rxBuf);
    NRF_UARTE0->PSEL.TXD               = 0x00000004; // 0x00000004==P0.04
    NRF_UARTE0->PSEL.RXD               = 0x00000003; // 0x00000003==P0.03
    NRF_UARTE0->CONFIG                 = 0x00000000; // 0x00000000==no flow control, no parity bits, 1 stop bit
    NRF_UARTE0->BAUDRATE               = 0x01D60000; // 0x01D60000==Baud115200
    
    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: CTS
    // .... .... .... .... .... .... .... ..B. B: NCTS
    // .... .... .... .... .... .... .... .C.. C: RXDRDY
    // .... .... .... .... .... .... ...D .... D: ENDRX
    // .... .... .... .... .... .... E... .... E: TXDRDY
    // .... .... .... .... .... ...F .... .... F: ENDTX
    // .... .... .... .... .... ..G. .... .... G: ERROR
    // .... .... .... ..H. .... .... .... .... H: RXTO
    // .... .... .... I... .... .... .... .... I: RXSTARTED
    // .... .... ...J .... .... .... .... .... J: TXSTARTED
    // .... .... .L.. .... .... .... .... .... L: TXSTOPPED
    // xxxx xxxx x0x0 0x0x xxxx xx01 0xx0 x000 
    //    0    0    0    0    0    1    0    0 0x00000100
    //NRF_UARTE0->INTENSET               = 0x00000100;
    NRF_UARTE0->ENABLE                 = 0x00000008; // 0x00000008==enable
    
    while(1) {
        //Smiley
        leds_all_off();
        leds_on(LEDS_SMILEY[i]);
        i = (i+1)%sizeof(LEDS_SMILEY);
        
        //__WFE();
    }
}

// RADIO
void RADIO_IRQHandler(void) {
    if (NRF_RADIO->EVENTS_DISABLED) {
        NRF_RADIO->EVENTS_DISABLED = 0;

        if (NRF_RADIO->CRCSTATUS != RADIO_CRCSTATUS_CRCSTATUS_CRCOk) {
            puts("Invalid CRC");
        } else {
                BUF_ACTION[5] = pdu[2];
                BUF_ACTION[6] = _compute_crc();

                NRF_UARTE0->EVENTS_ENDTX           = 0;
                NRF_UARTE0->TASKS_STARTTX          = 0x00000001;
                while(NRF_UARTE0->EVENTS_ENDTX== 0);
            
            printf("Received message %d : %s",pdu[1], &pdu[2]);
        }
    }
}
