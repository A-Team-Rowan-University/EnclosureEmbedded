/* onewire.h - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>


unsigned int ResetDevice(void)

void onewire_low(void);
void onewire_high(void);

inline void onewire_writezero(void);
inline void onewire_writeone(void);

void WriteByte(char byte);
uint16_t ReadData(void);


#endif