/*
 * SPI.h
 *
 * Created: 23/12/2016 7:02:12 μμ
 *  Author: Anestman
 */

#ifndef SPI_H_
#define SPI_H_

#define ACK 0x7E
#define NACK 0x77
#define START 0x70

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

void SPI_init_master(void);
void SPI_init_slave(void);
void SPI_write_long(long buffer);
uint16_t SPI_read_uint16(void);
void SPI_write_double(double value);
unsigned char SPI_tranceiver(unsigned char data);
char SPI_ASCII_tranceiver(char data);
void SPI_write_double_as_string(double data);
void SPI_write_string(char *string);

#endif /* SPI_H_ */
