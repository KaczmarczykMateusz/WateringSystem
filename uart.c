/*
 ============================================================================
 Name        :	uart.c
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega32, should work at Atmega8 too
 Description :	See uart.h
 ============================================================================
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "uart.h"

#include <stdlib.h>
// size of RX/TX buffers
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)

#define UART_RECEIVE_INTERRUPT   USART_RXC_vect
#define UART_TRANSMIT_INTERRUPT  USART_UDRE_vect

ISR(UART_RECEIVE_INTERRUPT)
/*************************************************************************
 Function: UART Receive Complete interrupt
 Purpose:  called when the UART has received a character
 **************************************************************************/
{
	uint16_t tmpHead;
	uint8_t data;
	uint8_t usr;
	uint8_t lastRxError;

// read UART status register and UART data register
	usr = UCSRA;
	data = UDR;

	lastRxError = (usr & (_BV(FE) | _BV(DOR)));

// calculate buffer index
	tmpHead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;

	if (tmpHead != UART_RxTail) {
// store new index
		UART_RxHead = tmpHead;
// store received data in buffer
		UART_RxBuf[tmpHead] = data;
	} else {
// error: receive buffer overflow
		lastRxError = UART_BUF_OVF >> 8;
	}
	UART_LastRxError = lastRxError;
}

ISR(UART_TRANSMIT_INTERRUPT)
/*************************************************************************
 Function: UART Data Register Empty interrupt
 Purpose:  called when the UART is ready to transmit the next byte
 **************************************************************************/
{
	uint16_t tmpTail;

	if (UART_TxHead != UART_TxTail) {
// calculate and store new buffer index
		tmpTail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmpTail;
// get one byte from buffer and write it to UART
		UDR = UART_TxBuf[tmpTail]; // start transmission
	} else {
// Tx buffer empty, disable UDRE interrupt
		UCSRB &= ~(1<<UDRIE);
	}
}

/*************************************************************************
 Function: uartInit()
 Purpose:  initialize UART and set baudrate
 Input:    baudrate using macro UART_BAUD_SELECT()
 Returns:  none
 **************************************************************************/
void uartInit(uint16_t baudrate) {
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UART_TxHead = 0;
		UART_TxTail = 0;
		UART_RxHead = 0;
		UART_RxTail = 0;
	}

	/* Set baud rate */
	if (baudrate & 0x8000) {
//		UCSRA = (1<<U2X);  //Enable 2x speed
		baudrate &= ~0x8000;
	}
	// in order to access UBRRH, the URSEL must be cleared
	UCSRC = ~(1<<URSEL);
	UBRRH = (uint8_t) (baudrate>>8);
	UBRRL = (uint8_t) baudrate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
	// in order to access UCSRC, the URSEL must be set

	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}

/*************************************************************************
 Function: uartGetc()
 Purpose:  return byte from circular buffer
 Returns:  lower byte:  received byte from circular buffer
 higher byte: last receive error
 **************************************************************************/
uint16_t uartGetc(void) {
	uint16_t tmpTail;
	uint8_t data;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (UART_RxHead == UART_RxTail) {
			return UART_NO_DATA; // no data available
		}
	}

// calculate / store buffer index
	tmpTail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;

	UART_RxTail = tmpTail;

// get data from receive buffer
	data = UART_RxBuf[tmpTail];

	return (UART_LastRxError << 8) + data;
}

/*************************************************************************
 Function: uartPeek()
 Purpose:  Returns the next byte (character) of incoming UART data without
 removing it from the ring buffer. That is, successive calls to
 uartPeek() will return the same character, as will the next
 call to uartGetc()
 Returns:  lower byte:  next byte in ring buffer
 higher byte: last receive error
 **************************************************************************/
uint16_t uartPeek(void) {
	uint16_t tmpTail;
	uint8_t data;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (UART_RxHead == UART_RxTail) {
			return UART_NO_DATA; // no data available
		}
	}

	tmpTail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;

// get data from receive buffer
	data = UART_RxBuf[tmpTail];

	return (UART_LastRxError << 8) + data;
}

/*************************************************************************
 Function: uartTxChar()
 Purpose:  write byte to circular buffer for transmitting via UART
 Input:    byte to be transmitted
 Returns:  none
 **************************************************************************/
void uartTxChar(uint8_t data) {
	uint16_t tmpHead;

	tmpHead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;

	// wait for free space in buffer
	while (tmpHead == UART_TxTail) {
	}

	UART_TxBuf[tmpHead] = data;
	UART_TxHead = tmpHead;

// enable UDRE interrupt
	UCSRB |= (1<<UDRIE);
}

/*************************************************************************
Function: uartTxHex()
Purpose: transmit hex as hex to UART
Input:	hex digit
Returns:  None
**************************************************************************/
void uartTxHex(uint8_t n) {
	if(((n>>4) & 15) < 10)
		uartTxChar('0' + ((n>>4)&15));
	else
		uartTxChar('A' + ((n>>4)&15) - 10);
	n <<= 4;
	if(((n>>4) & 15) < 10)
		uartTxChar('0' + ((n>>4)&15));
	else
		uartTxChar('A' + ((n>>4)&15) - 10);
}

/*************************************************************************
Function: uartTxInt(toChar passed)
Purpose:  Sends integer decoded as char
		  For use with user interface
Input:	  Digit to send, not larger than 999 999 999
**************************************************************************/
void uartTxInt(uint32_t passedValue) {
	char stringToPass[10+1] = "";
	ltoa(passedValue,stringToPass, 10);
	uartTxStr(stringToPass);
}

/*************************************************************************
 Function: uartTxStr()
 Purpose:  transmit string to UART
 Input:    string to be transmitted
 Returns:  none
 **************************************************************************/
void uartTxStr(const char *s) {
	while (*s) {
		uartTxChar(*s++);
	}
}

/*************************************************************************
 Function: uart_available()
 Purpose:  Determine the number of bytes waiting in the receive buffer
 Input:    None
 Returns:  Integer number of bytes in the receive buffer
 **************************************************************************/
uint16_t uartAvailable(void) {
	uint16_t ret;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		ret = (UART_RX_BUFFER_SIZE + UART_RxHead - UART_RxTail)
				& UART_RX_BUFFER_MASK;
	}
	return ret;
}

/*************************************************************************
 Function: uartFlush()
 Purpose:  Flush bytes waiting the receive buffer. Actually ignores them.
 Input:    None
 Returns:  None
 **************************************************************************/
void uartFlush(void) {
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UART_RxHead = UART_RxTail;
	}
}

/*************************************************************************
Function: uartCheck()
Purpose:  Combine bytes into string and check them for "enter" token.
Input:    Function for further parsing
Returns:  None
**************************************************************************/
void uartCheck (void (*action)(char *pBuf)) {
	static uint8_t i = 0;

	while(uartPeek() != UART_NO_DATA) {

		uartRxErr = (uartPeek() >> 8);
		receiveBufor[i] = (uartGetc() & ~(0xFF00));

		if(i < 100) {
			i++;
		} else {
			i = 0;
		}
		if(receiveBufor[i-1] == 13) {
			i = 0;
			action((char*)receiveBufor);
		}
	}
}
