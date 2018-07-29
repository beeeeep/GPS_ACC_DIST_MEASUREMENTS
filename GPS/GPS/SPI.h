/*
 * SPI.h
 *
 * Created: 23/12/2016 7:02:12 μμ
 *  Author: Anestman
 */ 

#ifndef SPI_H_
#define SPI_H_

#define BYTE_0 0x7E
#define BYTE_1 0x77
#define START 0x70
#define SPDR_BUFFER_SIZE 255


 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>

 uint8_t SPI_FLAG;

void SPI_init_master(void);
void SPI_init_slave(void);
void SPI_write_uint16(uint16_t buffer);
long SPI_read_long(void);
double	SPI_read_double(void);
unsigned char SPI_tranceiver (unsigned char data);


#endif /* SPI_H_ */
