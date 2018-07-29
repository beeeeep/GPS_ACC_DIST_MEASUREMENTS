/*
 * timer.h
 *
 * Created: 16/1/2017 10:19:33 μμ
 *  Author: Anestman
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
volatile static long timer_time;

void timer_init(void);

uint16_t timer_get_time(void);

void timer_reset(void);

#endif /* TIMER_H_ */