/*
============================================================================
Name        : lcd.h
Author      : Mateusz Kaczmarczyk
Version     : Microcontroller : Atmel AVR Atmega32, LCD with driver HD44780
Description : Should work on other AVR too (Atmega8 and 32 tested)
============================================================================
*/
#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>


// @brief PORT configuration block
#define LCD_RS_DIR		DDRB
#define LCD_RS_PORT 	PORTB
#define LCD_RS			(1 << PB0)

#define LCD_E_DIR		DDRB
#define LCD_E_PORT		PORTB
#define LCD_E			(1 << PB1)

#define LCD_DB4_DIR		DDRB
#define LCD_DB4_PORT	PORTB
#define LCD_DB4			(1 << PB2)

#define LCD_DB5_DIR		DDRB
#define LCD_DB5_PORT	PORTB
#define LCD_DB5			(1 << PB3)

#define LCD_DB6_DIR		DDRA
#define LCD_DB6_PORT	PORTA
#define LCD_DB6			(1 << PA2)

#define LCD_DB7_DIR		DDRC
#define LCD_DB7_PORT	PORTC
#define LCD_DB7			(1 << PC4)

// @brief Command definitions block
#define HD44780_CLEAR				0x01	//@brief wipe entire display from what was displayed

#define HD44780_HOME				0x02	//@brief Return to top left corner of screen

#define HD44780_EM_SHIFT_DECREMENT	(0x04 | 0x00)	//@brief Disable auto-scroll
#define HD44780_EM_SHIFT_INCREMENT	(0x04 | 0x01)	//@brief Enable auto-scroll
#define HD44780_EM_LEFT				(0x04 | 0x00)	//@brief Entry Mode (writing text) from right to right
#define HD44780_EM_RIGHT			(0x04 | 0x02)	//@brief Entry Mode (writing text) from left to right

#define HD44780_DISPLAY_OFF			(0x08 | 0x00)	//@brief Turn OFF LCD display
#define HD44780_DISPLAY_ON			(0x08 | 0x04)	//@brief Turn ON LCD display
#define HD44780_CURSOR_OFF			(0x08 | 0x00)	//@brief Turn OFF cursor
#define HD44780_CURSOR_ON			(0x08 | 0x02)	//@brief Turn ON cursor
#define HD44780_CURSOR_NOBLINK		(0x08 | 0x00)	//@brief Solid cursor without blinking
#define HD44780_CURSOR_BLINK		(0x08 | 0x01)	//@brief Cursor blinking mode

#define HD44780_SHIFT_CURSOR		(0x10 | 0x00)	//@brief move cursor to left or right (see below)
#define HD44780_SHIFT_DISPLAY		(0x10 | 0x08)	//@brief move display to left or right (see below)
#define HD44780_SHIFT_LEFT			(0x10 | 0x00)	//@brief choose to move display or cursor left
#define HD44780_SHIFT_RIGHT			(0x10 | 0x04)	//@brief choose to move display or cursor right

#define HD44780_FONT5x8				(0x20 | 0x00)	//@brief Font size small
#define HD44780_FONT5x10			(0x20 | 0x04)	//@brief Font size large
#define HD44780_ONE_LINE			(0x20 | 0x00)	//@brief Display with only one row
#define HD44780_TWO_LINE			(0x20 | 0x08)	//@brief Display with two rows
#define HD44780_FOUR_LINE			(0x20 | 0x0F)	//@brief Display with four rows

#define HD44780_4_BIT				(0x20 | 0x00)	//@brief 4-bit interface mode
#define HD44780_8_BIT				(0x20 | 0x0F)	//@brief 8-bit interface mode

#define HD44780_CGRAM_SET			0x40

#define HD44780_DDRAM_SET			0x80

/** @brief  Write to HD44780 data half byte by half byte while in 4 bit interface mode
 *  @param  older half of byte to send
 */
void _LCD_HalfOut(uint8_t writeHalf);

/** @brief  Sends byte to HD44780 using _LCD_HalfOut while 4-bit interface mode
 *  @param  One byte to send (no matter data  or command)
 */
void _LCD_WriteData(uint8_t);

/** @brief
 *  @param  Single command which is always one byte
 */
void LCD_WriteCommand(uint8_t);

/** @brief	Write text to the display
 *  @param  String as char array coded in ASCII
 */
void LCD_WriteText(char *);

/** @brief  Set position from which to start writing text
 *  @param  x: horizontal position
 	 	 	y: vertical position
 */
void LCD_GoTo(uint8_t, uint8_t);

/** @brief Clear everything what currently displayed at screen
 */
void LCD_Clear(void);

// @brief Start writing text to display from the beginning of it (right top corner)
void LCD_Home(void);

// @brief Initialise HD44780 driver based display
void LCD_Initalize(void);

/** @brief	Prints two simple rows with text (2x16) at the LCD from left to right
 *  @param  *row1	: buffer with text coded in ASCII to be displayed at first row of LCD
 *  @param  *row2	: buffer with text coded in ASCII to be displayed at second row of LCD
 *  @return	: None
 */
void printSimpleScreen(char *row1, char *row2);

#endif
