/*
 * Pinserial.h
 *
 * Created: 16/1/2017 1:55:33 πμ
 *  Author: Anestman
 */

#ifndef PINSERIAL_H_
#define PINSERIAL_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define DDR_MASTER DDRA
#define PORT_MASTER PORTA
#define PIN_MASTER PINA
#define DATA_PIN_MASTER PINA0
#define INT_PIN_MASTER INT0
#define ISR_VECT INT0_vect
#define DDR_SLAVE DDRA
#define PORT_SLAVE PORTA
#define PIN_SLAVE PINA
#define DATA_PIN_SLAVE PINA0
#define ACK_SEND_PIN_MASTER PINA1
#define ACK_READ_PIN_SLAVE PINA1
#define DATA_RDY_PIN_SLAVE PINA2

void Pin_serial_init_master(void);

void Pin_serial_init_slave(void);

void Pin_serial_write_byte(uint8_t byte);

uint8_t Pin_serial_read_byte(void);

uint16_t Pin_serial_read_word(void);

void Pin_serial_write_word(uint16_t word);

#endif /* PINSERIAL_H_ */