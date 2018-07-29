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
#define RX_BUFFER_SIZE 255

//**data types**//
#define data_lat 0b01100000
#define data_long 0b01010000
#define data_acc_distance 0b01000000
#define data_gps_distance 0b00110000
#define data_start_log 0b00100000
#define data_stop_log 0b00010000

uint8_t New_GPS_data_flag;

void USART_Init(void);

void USARTWrite(char c);

void serialWrite(char *data);

void serialWrite_int(int n);

void serialWrite_uint_data(unsigned long n, uint8_t data_type);

void serialWrite_double(double n, uint8_t num_of_dcmls);

//void serialWrite_float(float n);

void serialWrite_uint(uint16_t n);

void serialWrite_long(long n);

void serialWrite_ulong(unsigned long n);

void serialWrite_newline(void);

int serialRead_int(void);
char *serialRead_nemea_string(void);
//int serialRead_int_timed(void);

char getChar(void);

#endif /* TXSERIAL_H_ */
