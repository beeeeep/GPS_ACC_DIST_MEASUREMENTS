#include "txSerial.h"

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;
char data[RX_BUFFER_SIZE];

void USART_Init(void)
{
	UBRRH = (BRC >> 8);
	UBRRL = BRC;

	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);

	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

	sei();
}

void USARTWrite(char c)
{

	while (!(UCSRA & (1 << UDRE)))
		;

	UDR = c;
}

void serialWrite(char *data)
{
	uint8_t i = 0;
	while (data[i])
	{
		USARTWrite(data[i++]);
	}
}

void serialWrite_int(int n)
{

	char theIntAsString[7];
	int i;
	sprintf(theIntAsString, "%i", n);
	for (i = 0; i < strlen(theIntAsString); i++)
	{
		USARTWrite(theIntAsString[i]);
	}
}
void serialWrite_double(double n, uint8_t num_of_dcmls)
{

	static char buffer[31];
	char *print = dtostrf(n, 3, num_of_dcmls, buffer);
	serialWrite(print);
}

//void serialWrite_float(float n)
//{
//
//char thefloatAsString[7];
//int i;
//sprintf( thefloatAsString, "%f", n );
//for (i=0; i < strlen(thefloatAsString); i++)
//{
//USARTWrite(thefloatAsString[i]);
//}
//}
void serialWrite_uint(uint16_t n)
{

	char theIntAsString[7];
	int i;
	sprintf(theIntAsString, "%u", n);
	for (i = 0; i < strlen(theIntAsString); i++)
	{
		USARTWrite(theIntAsString[i]);
	}
}
void serialWrite_uint_data(unsigned long n, uint8_t data_type) // send a data<=2^24 bit (16.777.216)
{
	uint8_t buffer;
	for (uint8_t i = 0; i < 3; i++)
	{
		buffer = n;
		n = (n >> 8);
		while (!(UCSRA & (1 << UDRE)))
			;
		UDR = buffer;
	}
	buffer = (buffer && data_type);
	while (!(UCSRA & (1 << UDRE)))
		;
	UDR = buffer;
}

void serialWrite_long(long n)
{

	char theIntAsString[7];
	int i;
	sprintf(theIntAsString, "%ld", n);
	for (i = 0; i < strlen(theIntAsString); i++)
	{
		USARTWrite(theIntAsString[i]);
	}
}

void serialWrite_newline(void)
{
	serialWrite("\n\r");
}

int serialRead_int(void)
{
	int number;
	uint8_t i = 0;
	char Read_Char;
	char data[RX_BUFFER_SIZE];

	while (Read_Char != 0x0D) //Read until "Enter" is pressed
	{
		Read_Char = getChar();
		if (Read_Char != '\0')
		{
			data[i] = Read_Char;
			i++;
		}
	}

	number = atoi(data);

	return number;
}

char *serialRead_nemea_string(void)
{

	uint8_t i = 0;
	char Read_Char;
	char polling_char;

	while (polling_char != '$')
	{
		polling_char = getChar();
	}

	while (Read_Char != '*')
	{
		Read_Char = getChar();
		if (Read_Char != '\0')
		{

			data[i] = Read_Char;
			i++;
		}
	}

	//
	//}
	return data;
}
//
//int serialRead_int_timed(void)
//{
//unsigned int current_time;
//unsigned int old_time=0;
//int number;
//uint8_t i=0;
//char Read_Char;
//char data[RX_BUFFER_SIZE];
//
//while( Read_Char !=0x0D   ) //Read until "Enter" is pressed
//{
//current_time=millis_get();
//if (current_time-old_time>5000)
//{
//break;
//}
//
//
//Read_Char=getChar();
//if(Read_Char!='\0')
//{
//data[i]=Read_Char;
//i++;
//}
//
//}
//
//number=atoi(data);
//
//return number;
//
//}

char getChar(void)
{
	char ret = '\0';

	if (rxReadPos != rxWritePos)
	{
		ret = rxBuffer[rxReadPos];

		rxReadPos++;

		if (rxReadPos >= RX_BUFFER_SIZE)
		{
			rxReadPos = 0;
		}
	}

	return ret;
}

ISR(USART_RXC_vect)
{

	rxBuffer[rxWritePos] = UDR;

	if (rxBuffer[rxWritePos] == '$')
	{
		New_GPS_data_flag = 1;
	}
	rxWritePos++;

	if (rxWritePos >= RX_BUFFER_SIZE)
	{
		rxWritePos = 0;
	}
}