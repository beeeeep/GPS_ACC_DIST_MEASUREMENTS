/*
 * Pinserial.c
 *
 * Created: 16/1/2017 2:00:41 πμ
 *  Author: Anestman
 */

#include "Pinserial.h"

volatile static uint8_t int_flag;
void Pin_serial_init_master(void)
{
	uint8_t int_pin = INT_PIN_MASTER;
	DDR_MASTER |= (1 << ACK_SEND_PIN_MASTER);
	PORT_MASTER |= (1 << DATA_PIN_MASTER); //Enable internal pull-up

	if (int_pin == INT1)
	{
		GICR |= (1 << INT1);
		MCUCR |= (1 << ISC11); // Enable INT1 for FALLING edge
		PORTD |= (1 << PIND3); //Enable internal pull-up
	}
	else
	{
		GICR |= (1 << INT0);
		MCUCR |= (1 << ISC01); // Enable INT1 for FALLING edge
		PORTD |= (1 << PIND2); //Enable internal pull-up
	}
	sei();
}

void Pin_serial_init_slave(void)
{
	DDR_SLAVE = (1 << DATA_PIN_SLAVE) | (1 << DATA_RDY_PIN_SLAVE);
	PORT_SLAVE |= (1 << ACK_READ_PIN_SLAVE); //Enable internal pull-ups
}

void Pin_serial_write_byte(uint8_t byte)
{
	uint8_t buffer = byte;
	while (!(PIN_SLAVE & (1 << ACK_READ_PIN_SLAVE)))
		;
	for (uint8_t i = 0; i <= 7; i++)
	{
		buffer = (byte && 0x01);

		if (buffer)
		{
			PORT_SLAVE |= (1 << DATA_PIN_SLAVE);
		}
		else
		{
			PORT_SLAVE &= ~(1 << DATA_PIN_SLAVE);
		}
		byte = (byte >> 1);
		PORT_SLAVE &= ~(1 << DATA_RDY_PIN_SLAVE); //send data ready signal to master int pin
		while (!(PIN_SLAVE & (1 << ACK_READ_PIN_SLAVE)))
			;									 // wait for master ack
		PORT_SLAVE |= (1 << DATA_RDY_PIN_SLAVE); //stop sending signal
	}
}

uint8_t Pin_serial_read_byte(void)
{
	uint8_t byte;
	PORT_MASTER |= (1 << ACK_SEND_PIN_MASTER);
	_delay_ms(1);
	PORT_MASTER &= ~(1 << ACK_SEND_PIN_MASTER); //send ack to begin transmition

	for (uint8_t i = 0; i <= 7; i++)
	{
		while (!int_flag)
			;
		int_flag = 0;
		if ((PIN_MASTER & (1 << DATA_PIN_MASTER)))
			;
		{
			++byte;
		}
		byte = (1 << byte);
		PORT_MASTER |= (1 << ACK_SEND_PIN_MASTER);
		_delay_ms(1);
		PORT_MASTER &= ~(1 << ACK_SEND_PIN_MASTER);
	}

	return byte;
}

ISR(ISR_VECT)
{
	int_flag = 1;
}