/*
============================================================================
Name        : lcd.c
Author      : Mateusz Kaczmarczyk
Version     : Microcontroller : Atmel AVR Atmega32, LCD HD44780
Description : See header file
============================================================================
*/
#include "lcd.h"

/*************************************************************************
 Function: LCD_PowerUp()
 Purpose:  Pull up transistor supplying current LCD
 **************************************************************************/
void LCD_PowerUp(void) {
	LCD_POWER_PORT |= LCD_POWER;
}

/*************************************************************************
 Function:	LCD_PowerDown()
 Purpose: 	Pull down transistor supplying current LCD
 **************************************************************************/
void LCD_PowerDown(void) {
	LCD_POWER_PORT &= ~LCD_POWER;
}

/*************************************************************************
 Function: 	isLCD_On()
 Purpose:	Return LCD power state
 **************************************************************************/
uint8_t isLCD_On(void) {
	return LCD_POWER_PIN & LCD_POWER;
}

/*************************************************************************
 Function: 	_LCD_HalfOut()
 Purpose:	Send half byte
 Input:		First send older half of byte
 see:		_LCD_Write()
 **************************************************************************/
void _LCD_HalfOut(uint8_t writeHalf) {
if(writeHalf & 0x01)
	LCD_DB4_PORT |= LCD_DB4;
else
	LCD_DB4_PORT  &= ~LCD_DB4;

if(writeHalf & 0x02)
	LCD_DB5_PORT |= LCD_DB5;
else
	LCD_DB5_PORT  &= ~LCD_DB5;

if(writeHalf & 0x04)
	LCD_DB6_PORT |= LCD_DB6;
else
	LCD_DB6_PORT  &= ~LCD_DB6;

if(writeHalf & 0x08)
	LCD_DB7_PORT |= LCD_DB7;
else
	LCD_DB7_PORT  &= ~LCD_DB7;
}

/*************************************************************************
 Function: 	_LCD_Write()
 Purpose:	Write entire byte (data or command)
 Input:		Byte to write
 **************************************************************************/
void _LCD_Write(uint8_t dataToWrite) {
	LCD_E_PORT |= LCD_E;
	_LCD_HalfOut(dataToWrite >> 4);
	LCD_E_PORT &= ~LCD_E;
	LCD_E_PORT |= LCD_E;
	_LCD_HalfOut((dataToWrite & ~(0xF0)));
	LCD_E_PORT &= ~LCD_E;
	_delay_us(50);
}

/*************************************************************************
 Function: 	LCD_WriteCommand()
 Purpose:	Write command
 Input:		Command byte only
 **************************************************************************/
void LCD_WriteCommand(uint8_t commandToWrite) {
	LCD_RS_PORT &= ~LCD_RS;
	_LCD_Write(commandToWrite);
}

/*************************************************************************
 Function: 	_LCD_WriteData()
 Purpose:	Write data
 Input:		Data byte one by one
 **************************************************************************/
void _LCD_WriteData(uint8_t dataToWrite) {
	LCD_RS_PORT |= LCD_RS;
	_LCD_Write(dataToWrite);
}

/*************************************************************************
 Function: 	LCD_WriteText()
 Purpose:	Write text
 Input:		Char array
 **************************************************************************/
void LCD_WriteText(char * text) {
	while(*text)
		_LCD_WriteData(*text++);
}

/*************************************************************************
 Function: 	LCD_GoTo()
 Purpose:	Set from where to start writing at display
 Input:		x: horizontal position
 	 	 	y: vertical position
 **************************************************************************/
void LCD_GoTo(uint8_t x, uint8_t y) {
	LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

/*************************************************************************
 Function: 	LCD_Clear()
 Purpose:	Remove text from display
 **************************************************************************/
void LCD_Clear(void) {
	LCD_WriteCommand(HD44780_CLEAR);
	_delay_ms(2);
}

/*************************************************************************
 Function: 	LCD_Home()
 Purpose:	Return home (left, top)
 **************************************************************************/
void LCD_Home(void) {
	LCD_WriteCommand(HD44780_HOME);
	_delay_ms(2);
}

/*************************************************************************
 Function: 	LCD_Initalize()
 Purpose:	Display initialization
 **************************************************************************/
void LCD_Initalize(void) {
	LCD_POWER_DIR |= LCD_POWER;	//Initialise display PIN
	LCD_PowerUp();
	uint8_t i;
	LCD_DB4_DIR |= LCD_DB4;	// Configuration of pinout
	LCD_DB5_DIR |= LCD_DB5;
	LCD_DB6_DIR |= LCD_DB6;
	LCD_DB7_DIR |= LCD_DB7;
	LCD_E_DIR 	|= LCD_E;
	LCD_RS_DIR 	|= LCD_RS;
	_delay_ms(15); 			// wait for current stabilisation
	LCD_RS_PORT &= ~LCD_RS;
	LCD_E_PORT &= ~LCD_E;

	for(i = 0; i < 3; i++) {
		LCD_E_PORT |= LCD_E;	//  E = 1
		_LCD_HalfOut(0x03);		// 8 bit mode
		LCD_E_PORT &= ~LCD_E;	// E = 0
		_delay_ms(5);
	}

	LCD_E_PORT |= LCD_E;	// E = 1
	_LCD_HalfOut(0x02);		// 4 bit mode
	LCD_E_PORT &= ~LCD_E;	// E = 0

	_delay_ms(1);
	LCD_WriteCommand(HD44780_FONT5x8 | HD44780_TWO_LINE | HD44780_4_BIT);				// 4 BIT interface, 2 lines, font size 5x7
	LCD_WriteCommand(HD44780_DISPLAY_OFF);	// turn off display
	LCD_WriteCommand(HD44780_CLEAR);		// clear DDRAM
	_delay_ms(2);
	LCD_WriteCommand(HD44780_EM_RIGHT);		// address incrementation and shift of cursor
	LCD_WriteCommand(HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);	// turn on display without cursor nor blinking
}

/*************************************************************************
 Function: printSimpleScreen()
 Purpose: Print two simple rows with text (2x16) at the LCD from left to right
 Input: Two rows of text as max 16 char arrays (each one)
 **************************************************************************/
void printSimpleScreen(char *row1, char *row2) {
	LCD_Clear();

	if(row1) {		// Write first line
		LCD_GoTo(0,0);
		LCD_WriteText(row1);
	}

	if(row2) {		// Write second line
		LCD_GoTo(0,1);
		LCD_WriteText(row2);
	}
}
