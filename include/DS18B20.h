#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>
#include <msp430g2553.h>

#define DS1820_OUT 					P2OUT
#define DS1820_DIR 					P2DIR
#define DS1820_SEL					P2SEL
#define DS1820_IN					P2IN
#define DS1820_DATA_IN_PIN          BIT4

#define DS1820_SKIP_ROM             0xCC
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_CONVERT_T            0x44

float GetData(void);

#endif