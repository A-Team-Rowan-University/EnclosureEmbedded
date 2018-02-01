/* File:    lcd_4bit.h
*
* Author:  David Zemon
* Project: LCD_Backpack
*/

#ifndef LCD_4BIT_H_
#define LCD_4BIT_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>

#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

/************************
*** Public Functions ***
************************/
/**
* \brief	Initialize the LCD module
*
* \param	controlPortEnable	Port enabler constant selected from driverlib/sysctl.h
* 					(i.e., SYSCTL_PERIPH_GPIOB)
* \param	controlPort
* 								Port base constant selected from driverlib/gpio.h
* 					(i.e., GPIO_PORTB_BASE)
* \param	rs					Pin mask for register select
* \param	rw					Pin mask for register write
* \param	en					Pin mask for enable
*
* \param	dataPortEnable		Port enabler constant selected from driverlib/sysctl.h
* 					(i.e., SYSCTL_PERIPH_GPIOC)
* \param	dataPort			Port base constant selected from driverlib/gpio.h
* 					(i.e., GPIO_PORTC_BASE)
* \param	dataMask			Pin mask for data pins
* \param	dataLSB				Pin number of the LSB pin (NOTE: data pins must be connected
* 					with the LSB pin from the uC connected to the LSB pin on the LCD)
*/
void lcd_open(const uint32_t controlPortEnable, const uint32_t controlPort,
	const uint32_t rs, const uint32_t rw, const uint32_t en,
	const uint32_t dataPortEnable, const uint32_t dataPort, const uint32_t dataMask,
	uint32_t dataLSB);

/**
* \brief	Clear the screen of all characters
*/
void lcd_ioctl_clear(void);

/**
* \brief	Print a single char to the LCD and increment the pointer (automatic)
*
* \param	c	Individual char to be printed
*/
void lcd_ioctl_putchar(const char c);

/**
* \brief		Print a string to the LCD
*
* \detailed	Via a series of calls to lcd_ioctl_putchar, prints each character
* 				individually; Will not auto-wrap lines that extend past the screen
*
* \param	*s	Address where c-string can be found (must be null-terminated)
*/
void lcd_ioctl_puts(const char* s);

/**
* \brief		Move the cursor to a specified column and row
*
* \detailed	Please note! The row can be one of 0, 1, 2, or 3. You do not need to use
* 				the HD44780 standard of 0, 4, 1, 5, etc
*/
void lcd_ioctl_move(const uint8_t row, const uint8_t col);

/*************************
*** Private Functions ***
*************************/
/**
* \brief	Send a control command to the LCD module
*
* \param	cmd		8-bit command to send to the LCD
*/
static void lcd_ioctl_sendCmd(const uint8_t cmd);

/**
* \brief	Toggle the enable pin, inducing a write to the LCD's register
*/
static inline void lcd_ioctl_blink(void);

#endif /* LCD_4BIT_H_ */