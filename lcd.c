//	lcd.c

#include "lcd.h"

// Send half byte
void _LCD_HalfOut(unsigned char writeHalf) {
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

// write entire byte (data or command)
void _LCD_Write(unsigned char dataToWrite) {
	LCD_E_PORT |= LCD_E;
	_LCD_HalfOut(dataToWrite >> 4);
	LCD_E_PORT &= ~LCD_E;
	LCD_E_PORT |= LCD_E;
	_LCD_HalfOut(dataToWrite);
	LCD_E_PORT &= ~LCD_E;
	_delay_us(50);
}

// write command
void LCD_WriteCommand(unsigned char commandToWrite) {
	LCD_RS_PORT &= ~LCD_RS;
	_LCD_Write(commandToWrite);
}

// write data
void LCD_WriteData(unsigned char dataToWrite) {
	LCD_RS_PORT |= LCD_RS;
	_LCD_Write(dataToWrite);
}

// Write text
void LCD_WriteText(char * text) {
	while(*text)
		LCD_WriteData(*text++);
}

// Set starting position on display
void LCD_GoTo(unsigned char x, unsigned char y) {
	LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

// Clearing text from display
void LCD_Clear(void) {
	LCD_WriteCommand(HD44780_CLEAR);
	_delay_ms(2);
}

// Set defaulut starting point at display
void LCD_Home(void) {
	LCD_WriteCommand(HD44780_HOME);
	_delay_ms(2);
}

//  display initialization
void LCD_Ini(void) {
	unsigned char i;
	LCD_DB4_DIR |= LCD_DB4; // Configuration of pinout
	LCD_DB5_DIR |= LCD_DB5; 
	LCD_DB6_DIR |= LCD_DB6; 
	LCD_DB7_DIR |= LCD_DB7; 
	LCD_E_DIR 	|= LCD_E;   
	LCD_RS_DIR 	|= LCD_RS;  
	_delay_ms(15); // wait for current stabilization 
	LCD_RS_PORT &= ~LCD_RS; 
	LCD_E_PORT &= ~LCD_E;  

	for(i = 0; i < 3; i++) {
		LCD_E_PORT |= LCD_E; //  E = 1
		_LCD_HalfOut(0x03); // 8 bit mode
		LCD_E_PORT &= ~LCD_E; // E = 0
		_delay_ms(5); 
	}

	LCD_E_PORT |= LCD_E; // E = 1
	_LCD_HalfOut(0x02); // 4 bit mode
	LCD_E_PORT &= ~LCD_E; // E = 0

	_delay_ms(1); 
	LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // turn off display
	LCD_WriteCommand(HD44780_CLEAR); // clear DDRAM
	_delay_ms(2);
	LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// address incrementation and shift of coursor
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // turn on display without coursr nor blinking
}
