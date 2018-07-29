#ifndef TXSERIAL_H_
#define TXSERIAL_H_
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BAUD 57600
#define BRC ((F_CPU / (16UL * BAUD)) - 1)
#define RX_BUFFER_SIZE 128
void USART_Init(void);

void USARTWrite(char c);

void serialWrite(char *data);

void serialWrite_int(int n);

void serialWrite_double(double n);

void serialWrite_float(float n);

void serialWrite_uint(int n);

void serialWrite_long(long n);

void serialWrite_ulong(unsigned long n);

void serialWrite_newline(void);

int SerialRead_int(void);

int SerialRead_int_timed(void);

char getChar(void);

#endif /* TXSERIAL_H_ */
