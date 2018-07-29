/****************************************************************
LCD.h LCD Driver code
Author: Athanasiadis Anestis





*****************************************************************/
#ifndef LCD_H_
#define LCD_H_

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#define lcd_port PORTA
#define lcd_port_dir DDRA
#define LCD_RDATA 0b00010000 //Register select pin: Portx,4
#define LCD_READ 0b00100000  // Read/!Write pin: Portx,5
#define LCD_EN 0b01000000    // LCD Enable pin: Portx,6
/********************commands************************************/
#define clr_display 0b00000001
#define cursor_home 0b00000010
#define cursor_left 0x10  //Moves cursor left by 1 character
#define cursor_right 0x14 //Moves cursor right by 1 character
#define entry_mode_leftS 0b00000111
#define entry_mode_rightS 0b00000101
#define display_on 0b00001100
#define display_off 0b00001000
#define cursor_on 0b00001110
#define cursor_off 0b00001100
#define cursor_line1 0x80
#define cursor_line2 0xC0
#define cursor_flash 0b00001111
#define CGRAM_address 0b01000000 //address should be added with OR
#define DDRAM_address 0b10000000 //address should be added with OR

/**********************functions*************************************/

void lcd_reset(void);

void lcd_cmd(char cmd);

void lcd_data(unsigned char data);

void lcd_write(char *a);

void lcd_init(void);

void lcd_cursor_pos(int col, int line);

void lcd_write_uint(unsigned int i);

void lcd_write_double(double i, uint8_t num_of_dcmls);

#endif /* LCD_H_ */