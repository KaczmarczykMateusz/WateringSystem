/*
 ============================================================================
 Name        :	uart.h
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega32, should work at Atmega8, 16, 323  too
 Description :	Functions for transmitting and receiving data through the on board UART.
  				UART generate interrupt if finished transmitting or receiving a byte.
				The interrupt handling routines use circular buffers
				for buffering received and transmitted data.

				The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE constants define
				the size of the circular buffers in bytes.

				You need to define buffer sizes as a symbol in your compiler settings or in uart.h
 ============================================================================
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/io.h>

#include "dataParsing.h"

// Set size of receive and transmit buffers
#define UART_RX_BUFFER_SIZE 128 /**< Size of the circular receive buffer, must be power of 2 */
#define UART_TX_BUFFER_SIZE 128 /**< Size of the circular transmit buffer, must be power of 2 */

// Check buffer sizes are not too large for 8-bit positioning
#if (UART_RX_BUFFER_SIZE > 256 & !defined(USART_LARGE_BUFFER))
	#error "Buffer too large, please use -DUSART_LARGE_BUFFER switch in compiler options"
#endif

// Check buffer sizes are not too large for *_LARGE_BUFFER operation (16-bit positioning)
#if (UART_RX_BUFFER_SIZE > 32768)
	#error "Buffer too large, maximum allowed is 32768 bytes"
#endif

/** @brief  UART Baudrate Setting
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)

/** @brief  UART Baudrate Setting for ATmega double speed mode
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) ((((xtalCpu)+4UL*(baudRate))/(8UL*(baudRate))-1)|0x8000)

/* test if the size of the circular buffers fits into SRAM */
#if ((UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60))
	#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

// high byte error return code of uartGetc()
#define UART_FRAME_ERROR      0x0800              // Framing Error by UART
#define UART_OVERRUN_ERROR    0x0400              // Overrun condition by UART
#define UART_BUF_OVF  		  0x0200              // receive circular buffer overflow
#define UART_NO_DATA          0x0100              // no receive data available

//  Module global variables
static volatile uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];

static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static volatile uint8_t UART_LastRxError;

uint8_t receiveBufor[100];
uint8_t uartRxErr;

// function prototypes

/**
   @brief   Initialize UART and set baudrate
   @param   Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
extern void uartInit(uint16_t baudrate);

/**
 *  @brief   Get received byte from circular buffer
 *
 * Returns in the lower byte the received character and in the
 * higher byte the last receive error.
 * UART_NO_DATA is returned when no data is available.
 *
 *  @return  lower byte:  received byte from circular buffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA
 *             <br>no receive data available
 *           - \b UART_BUF_OVF
 *             <br>Receive circular buffer overflow.
 *             We are not reading the receive buffer fast enough,
 *             one or more received character have been dropped
 *           - \b UART_OVERRUN_ERROR
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR
 *             <br>Framing Error by UART
 */
extern uint16_t uartGetc(void);

/**
 *  @brief   Peek at next byte in circular buffer
 *
 * Returns the next byte (character) of incoming UART data without removing it from the
 * internal ring buffer. That is, successive calls to uartPeek() will return the same
 * character, as will the next call to uartGetc().
 *
 * UART_NO_DATA is returned when no data is available.
 *
 *  @return  lower byte:  next byte in circular buffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA
 *             <br>no receive data available
 *           - \b UART_BUF_OVF
 *             <br>Receive circular buffer overflow.
 *             We are not reading the receive buffer fast enough,
 *             one or more received character have been dropped
 *           - \b UART_OVERRUN_ERROR
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR
 *             <br>Framing Error by UART
 */
extern uint16_t uartPeek(void);

/**
 *  @brief   Put byte to circular buffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uartPutc(uint8_t data);

/**
 *  @brief   Put string to circular buffer for transmitting via UART
 *
 *  The string is buffered by the UART library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 *
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uartPuts(const char *s);

/**
 * @brief    Put string from program memory to circular buffer for transmitting via UART.
 *
 * The string is buffered by the UART library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uartPuts_P
 */
extern void uartPuts_p(const char *s);

/**
 * @brief    Macro to automatically put a string constant into program memory
 * \param    __s string in program memory
 */
#define uartPuts_P(__s)       uartPuts_p(PSTR(__s))

/**
 *  @brief   Return number of bytes waiting in the receive buffer
 *  @return  bytes waiting in the receive buffer
 */
extern uint16_t uartAvailable(void);

/**
 *  @brief   Flush bytes waiting in the receive buffer
 */
extern void uartFlush(void);

/**
 * @brief    ombine bytes into string and check them for "enter" token.
 * \param    Function for further parsing
 */
void uartCheck(void (*action)(char *pBuf));

void uwrite_hex(unsigned char n);

#endif
