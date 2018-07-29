/*
 * timer.c
 *
 * Created: 16/1/2017 10:19:53 μμ
 *  Author: Anestman
 */

#include "timer.h"

volatile static uint8_t int_counter;

void timer_init(void)
{
	TCCR1B |= (1 << CS10);
	TIMSK |= (1 << TOIE1);
}

uint16_t timer_get_time(void)
{
	return 0;
}
void timer_reset(void)
{
	timer_time = 0;
}

ISR(TIMER1_OVF_vect)
{
	if (int_counter == 10)
	{
		timer_time += 41;
		int_counter = 0;
	}
}