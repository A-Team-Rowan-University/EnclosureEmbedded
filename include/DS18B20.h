#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>
#include <msp430g2553.h>

#define DS18B20_OUT 					P2OUT
#define DS18B20_DIR 					P2DIR
#define DS18B20_SEL					    P2SEL
#define DS18B20_IN					    P2IN
#define DS18B20_DATA_IN_PIN             BIT4

#define DS18B20_SKIP_ROM             0xCC
#define DS18B20_READ_SCRATCHPAD      0xBE
#define DS18B20_CONVERT_T            0x44

float GetData(void);

#endif
