/*
* GPS_MPU.c
*
* Created: 23/12/2016 6:55:10 μμ
*  Author: Anestman
*/

#include <avr/io.h>
#include "mpu9150.h"
#include "txSerial.h"
#include <util/delay.h>
#include "SPI.h"
#include "millis.h"
#include "DREC.h"

xyz mpu_reading;
uint16_t counter;
double distance;
xyz vectors;
uint16_t spi_buffer;

volatile uint8_t log_button;
uint8_t send_array[2];
uint8_t counter_2;
uint16_t init_counter;

millis_t oldtime;
millis_t newtime;
double DTtime;
void setup(void)
{
	DDRB |= (1 << PINB0);
	DDRA |= (1 << PINA0);

	millis_init();
	USART_Init();
	SPI_init_master();

	sei();

	serialWrite("Starting...");
	serialWrite_newline();

	mpu9150_setup(0, DLPF_5_Hz);
	_delay_ms(500);
	mpu9150_acc_setup(MR_2_g, 0);
	_delay_ms(500);
	mpu9150_gyro_setup(MR_500, 0);

	while (init_counter < 1200)
	{
		newtime = millis_get();
		DTtime = newtime - oldtime;
		//*******************************************************// init filters
		if (DTtime >= 5)
		{
			init_counter++;
			oldtime = newtime;
			DTtime *= 0.001;
			DREC_get_PDR_distance(DTtime);
			//*******************************************************// init filters
		}
	}

	GICR |= (1 << INT1); //EXT INT LOW LEVEL
}

int main(void)
{
	setup();
	_delay_ms(2000);

	while (1)
	{
		DREC_param_reset();

		PORTB |= (1 << PINB0);
		_delay_ms(1000);
		PORTB &= ~(1 << PINB0);
		while (log_button == 0)
			;
		PORTA |= (1 << PINA0); //Send start signal to ATMEGA32(GPS)
		PORTB |= (1 << PINB0); // Turn on Led
		newtime = millis_get();
		DTtime = newtime - oldtime;
		mpu9150_param_reset();
		serialWrite("log start.");
		serialWrite_newline();

		while (log_button)
		{
			newtime = millis_get();

			DTtime = newtime - oldtime;
			if (DTtime >= 5)
			{
				oldtime = newtime;
				DTtime *= 0.001;
				distance = DREC_get_PDR_distance(DTtime);
				++counter;
				serialWrite_double(distance);
				serialWrite_newline();
			}
			if (counter >= 500)
			{
				SPI_write_double_as_string(distance);
				counter = 0;
			}
		}

		SPI_write_double_as_string(distance);
		PORTA &= ~(1 << PINA0); //Send stop signal to ATMEGA32(GPS)
		PORTB &= ~(1 << PINB0); //TURN OFF LED
		serialWrite("log end.");
	}
}

ISR(INT1_vect)
{
	_delay_ms(500);
	if (log_button)
	{
		log_button = 0;
	}
	else
	{
		log_button = 1;
	}
}