#include "LCD.h"

void lcd_reset(void) //LCD Startup sequence
{
	lcd_port_dir = 0xFF;
	_delay_ms(20);
	lcd_port = (0b00000011 | LCD_EN); //enable on + command
	lcd_port = 0b00000011;			  //enable off

	_delay_ms(10);
	lcd_port = (0b00000011 | LCD_EN); //enable on + command
	lcd_port = 0b00000011;			  //enable off
	_delay_ms(1);
	lcd_port = (0b00000011 | LCD_EN);
	lcd_port = 0b00000011;
	_delay_ms(1);
	lcd_port = (0b00000010 | LCD_EN);
	lcd_port = 0b00000010;
	_delay_ms(1);
}

void lcd_cmd(char cmd) //LCD commands function
{
	lcd_port = ((cmd >> 4) & 0x0F) | LCD_EN; //rshift high nibble to send it first
	lcd_port = ((cmd >> 4) & 0x0F);
	lcd_port = (cmd & 0x0F) | LCD_EN; //send low nibble
	lcd_port = (cmd & 0x0F);
	_delay_ms(2);
}

void lcd_data(unsigned char data) //Display single character
{
	lcd_port = (((data >> 4) & 0x0F) | LCD_EN | LCD_RDATA); //rshift high nibble to send it first, data register enabled
	lcd_port = (((data >> 4) & 0x0F) | LCD_RDATA);

	lcd_port = ((data & 0x0F) | LCD_EN | LCD_RDATA); //send low nibble, data register enabled
	lcd_port = ((data & 0x0F) | LCD_RDATA);
	_delay_ms(2);
}

void lcd_write(char *a) // Write string on the lcd
{
	unsigned int i;
	for (i = 0; a[i] != 0; i++)
		lcd_data(a[i]);
}

void lcd_init(void)
{
	lcd_reset();		 // Call LCD reset;
	lcd_cmd(0x28);		 // 4-bit mode – 2 line – 5×7 font
	lcd_cmd(0b00001100); // Display on, no cursor – no blink.
	lcd_cmd(0b00010100); // Automatic Increment – No Display shift.
	lcd_cmd(0x80);		 // Address DDRAM with 0 offset 80h.
	lcd_cmd(clr_display);
}

void lcd_cursor_pos(int col, int line) //moves cursor to position given by user
{
	unsigned int j;

	if (line == 2)
	{
		lcd_cmd(cursor_line2);
	}
	else
	{
		lcd_cmd(cursor_line1);
	}

	for (j = 1; j < col; j++)
	{
		lcd_cmd(cursor_right);
	}
}

void lcd_write_uint(unsigned int i) // changes intergers to char and prints them
{
	char str[10];

	sprintf(str, "%d", i);
	lcd_write(str);
}

void lcd_write_double(double i, uint8_t num_of_dcmls)
{
	static char buffer[10];
	char *print = dtostrf(i, 3, num_of_dcmls, buffer);
	lcd_write(print);
}