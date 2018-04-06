#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>
#include <msp430g2553.h>

#define DS18B20_OUT 					P2OUT
#define DS18B20_DIR 					P2DIR
#define DS18B20_SEL					    P2SEL
#define DS18B20_IN					    P2IN
#define DS18B20_DATA_IN_PIN             BIT0

#define DS18B20_SKIP_ROM             0xCC
#define DS18B20_READ_SCRATCHPAD      0xBE
#define DS18B20_CONVERT_T            0x44
#define DS18B20_COPY                 0x48

float GetData(void);
static void InitDS18B20(void);
static unsigned int ResetDevice(void);
static void onewire_low(void);
static void onewire_high(void);
static inline void onewire_writezero(void);
static inline void onewire_writeone(void);
unsigned int onewire_readBit (void);
static void WriteByte(char byte);
static uint16_t ReadData(void);

#endif
extern uint8_t ds18b20convert( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );
extern uint8_t ds18b20rsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t *sp );
extern uint8_t ds18b20wsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t th, uint8_t tl, uint8_t conf );
extern uint8_t ds18b20csp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );
extern uint8_t ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, int16_t *temperature ) ;
extern uint8_t ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

#endif