#include "SPI.h"

volatile static uint8_t SPDR_value[SPDR_BUFFER_SIZE];
volatile static uint8_t SPDR_Write_Pos;
static uint8_t SPDR_Pos_buffer;

void SPI_init_slave(void)
{
	DDRB |= (1 << PINB6);										 //MISO as OUTPUT
	SPCR = (1 << SPE) | (1 << SPIE) | (1 << SPR0) | (1 << SPR1); //Enable SPI
	SPI_FLAG = 0;
}

void SPI_init_master(void)
{
	DDRB = (1 << PINB7) | (1 << PINB5);

	// Enable SPI, Set as Master
	// Prescaler: Fosc/16, Enable Interrupts
	//The MOSI, SCK pins are as per ATMega8
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

unsigned char spi_tranceiver(unsigned char data)
{
	SPDR = data; //Load data into buffer
	while (!(SPSR & (1 << SPIF)))
		;		   //Wait until transmission complete
	return (SPDR); //Return received data
}

long SPI_read_long(void)
{

	long word;
	long SPDR_Read_Pos;
	long byte[4];
	byte[0] = 0;
	byte[1] = 0;
	byte[2] = 0;
	byte[3] = 0;
	// 	while(byte[0]!='$')
	// 	{
	// 	byte[0]=spi_tranceiver(0x10);
	// 	}
	// 	byte[0] = spi_tranceiver(0x01);
	// 	byte[1] = spi_tranceiver(0x02);
	// 	word=byte[1]|(byte[0]<<8);
	// if(SPDR_Read_Pos<=(SPDR_Write_Pos-3))
	// {
	// 	while(SPDR_value[SPDR_Read_Pos++]!='$');
	// 	byte[0]=SPDR_value[SPDR_Read_Pos];
	// 	SPDR_Read_Pos++;
	// 	byte[1]=SPDR_value[SPDR_Read_Pos];
	// 	SPDR_Read_Pos++;
	// 	word=byte[1]|(byte[0]<<8);
	// }
	while (SPDR_Read_Pos != SPDR_Write_Pos)
	{
		SPDR_Read_Pos = SPDR_Pos_buffer + 1;
		byte[0] = SPDR_value[SPDR_Read_Pos];
		SPDR_Read_Pos++;
		byte[1] = SPDR_value[SPDR_Read_Pos];
		SPDR_Read_Pos++;
		byte[2] = SPDR_value[SPDR_Read_Pos];
		SPDR_Read_Pos++;
		byte[3] = SPDR_value[SPDR_Read_Pos];
		SPDR_Read_Pos++;
	}

	word = (byte[3] << 24) | (byte[2] << 16) | (byte[1] << 8) | byte[0];

	return (word);
}

double SPI_read_double(void)
{
	double double_value;
	double_value = SPI_read_long();
	double_value /= 100;
	return (double_value);
}

ISR(SPI_STC_vect)
{

	SPDR_value[SPDR_Write_Pos] = SPDR;

	if (SPDR_value[SPDR_Write_Pos] == '$')
	{
		SPI_FLAG = 1;
		SPDR_Pos_buffer = SPDR_Write_Pos;
	}
	SPDR_Write_Pos++;
	if (SPDR_Write_Pos >= SPDR_BUFFER_SIZE)
	{
		SPDR_Write_Pos = 0;
	}
}

//
//
//ISR(SPI_STC_vect)
//{
//SPDR_value=SPDR;
//SPDR=0x7E;
//if(SPDR_value=='$')
//{
//SPDR_Write_Pos=0;
//}
//else
//{
//SPI_byte[SPDR_Write_Pos]=SPDR_value;
//SPDR_Write_Pos++;
//}
//if(i>=3)
//{
//distance=SPI_byte[0]|(SPI_byte[1]<<8|SPI_byte[2]<<16|SPI_byte[3]<<24);
//SPI_FLAG=1;
//}
//}