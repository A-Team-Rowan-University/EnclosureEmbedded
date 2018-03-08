#include "DS1820.h"


unsigned int ResetDevice(void){}

void onewire_low(void);
void onewire_high(void);

inline void onewire_writezero(void);
inline void oneire_writeone(void);

void WriteByte(char byte);
uint16_t ReadData(void);
