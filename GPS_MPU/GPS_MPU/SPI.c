#include "SPI.h"

void SPI_init_slave(void)
{
	DDRB |= (1 << PINB6); //MISO as OUTPUT
	SPCR |= (1 << SPE);   //Enable SPI
	sei();
}

void SPI_init_master(void)
{
	DDRB = (1 << PINB7) | (1 << PINB5);

	// Enable SPI, Set as Master
	// Prescaler: Fosc/16, Enable Interrupts
	//The MOSI, SCK pins are as per ATMega8
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
}
char SPI_ASCII_tranceiver(char data)
{

	SPDR = data;
	//Load data into buffer
	while (!(SPSR & (1 << SPIF)))
		;		   //Wait until transmission complete
	return (SPDR); //Return received data
}

unsigned char spi_tranceiver(unsigned char data)
{
	SPDR = data; //Load data into buffer
	while (!(SPSR & (1 << SPIF)))
		;		   //Wait until transmission complete
	return (SPDR); //Return received data
}

// uint16_t SPI_read_uint16(void)
// {
//
// 	uint16_t word;
// 	uint8_t byte0;
// 	uint8_t byte1;
// 	byte0 = spi_tranceiver();
//
// 	byte1=(spi_tranceiver());
// 	word=byte0+(byte1<<8);
//
// 	return(word);
// }

void SPI_write_string(char *string)
{
	uint8_t i = 0;
	SPI_ASCII_tranceiver('$');
	while (string[i])
	{

		SPI_ASCII_tranceiver(string[i++]);
		_delay_us(600);
		;
	}

	SPI_ASCII_tranceiver('*');
}

void SPI_write_double_as_string(double data)
{
	static char buffer[31];

	char *print = dtostrf(data, 3, 3, buffer);

	SPI_write_string(print);
}

void SPI_write_long(long buffer)
{
	unsigned char data_array[3];
	data_array[0] = buffer;
	data_array[1] = (buffer >> 8);
	data_array[2] = (buffer >> 16);
	data_array[3] = (buffer >> 24);
	uint8_t data;
	spi_tranceiver('$');
	_delay_us(600);
	data = spi_tranceiver(data_array[0]);
	_delay_us(600);
	data = spi_tranceiver(data_array[1]);
	_delay_us(600);
	data = spi_tranceiver(data_array[2]);
	_delay_us(600);
	data = spi_tranceiver(data_array[3]);
}

void SPI_write_double(double value)
{
	value *= 100;
	SPI_write_long((long)value);
}