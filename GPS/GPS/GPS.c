/*
* GPS.c
*
* Created: 18/12/2016 1:21:48 πμ
*  Author: Anestman
*/
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "txSerial.h"
#include "nmea.h"
#include "LCD.h"
#include "SPI.h"

extern char GPSTime[6];
extern char Lat[9];
extern char Long[11];
extern char NS[1];
extern char EW[1];
extern char FixQuality[4];
extern char Altitude[7];
extern char No_of_sats[3];

//GPRMC data
extern char Speed[5]; //in knots
extern char Heading[5];
extern char GPSDate[6];
struct_Location New_Location;
struct_Location Old_Location;
struct_Location Location_Sum;

double n_of_samples;
uint8_t sample_no;
uint8_t data;
double gps_distance;
double gps_distance_t;
uint8_t init_log;
volatile static uint8_t log_button;
uint8_t log_end_flag;
double mpu_distance;

uint8_t pinmask;

void setup(void)
{
	//     GICR|=(1<<INT1);
	//     MCUCR|=(1<<ISC11);
	//	DDRD=(1<<PIND7);
	//	DDRB|=(1<<PINB0);
	//	PORTB|=(1<<PINB0);
	//PORTD|=(1<<PIND3);

	USART_Init();
	lcd_init();
	//SPI_init_slave();

	sei();

	serialWrite("starting");
	serialWrite_newline();

	lcd_cmd(clr_display);
	lcd_cursor_pos(1, 1);
	lcd_write("starting..");
	_delay_ms(1000);
	lcd_cursor_pos(1, 1);

	lcd_cmd(clr_display);
	lcd_cursor_pos(1, 1);
	lcd_write("Waiting 4 signal");
	serialWrite("Waiting 4 signal...");
	serialWrite_newline();

	pinmask = PIND & 0b01000000;
	if (pinmask == 0b01000000)
	{

		while (New_Location.Latitude == 0)
		{

			if (New_GPS_data_flag == 1)
			{
				New_GPS_data_flag = 0;
				data = nmeaProcess();
				// String Parsing for NMEA GMC and GPA
				if (data == 6) // if GMC or GPA
				{
					nmea_Normalise_Data(Lat, Long, NS, EW, Altitude, &New_Location);
				}
			}
			_delay_us(10);
		}
	}

	lcd_cmd(clr_display);
	lcd_cursor_pos(1, 2);
	serialWrite("ready");
	lcd_write("ready");
	serialWrite_newline();

	//*******************************************************//

	_delay_ms(2000);
}

int main(void)
{
	setup();

	while (1)
	{
		log_button = 1;
		while (log_button == 0)
			;
		gps_distance = 0;
		mpu_distance = 0;
		sample_no = 0;
		init_log = 0;
		serialWrite("wait...");
		serialWrite_newline();
		lcd_cmd(clr_display);
		lcd_cursor_pos(1, 1);
		lcd_write("wait...");
		serialWrite("Logging...");
		serialWrite_newline();
		lcd_cmd(clr_display);
		lcd_cursor_pos(1, 1);
		lcd_write("Logging...");

		//	PORTB^=(1<<PINB0);

		// 		_delay_us(50);
		// 		PORTB^=(1<<PINB0);

		while (log_button == 1)
		{

			if (New_GPS_data_flag == 1)
			{
				New_GPS_data_flag = 0;

				data = nmeaProcess(); // String Parsing for NMEA GMC and GPA
				if (data == 1)		  // if GMC or GPA
				{

					nmea_Normalise_Data(Lat, Long, NS, EW, Altitude, &New_Location);

					Location_Sum.Latitude += New_Location.Latitude;
					Location_Sum.Longtitude += New_Location.Longtitude;
					Location_Sum.Altitude += New_Location.Altitude;

					//serialWrite_uint_data((unsigned long)(New_Location.Latitude*100000),data_lat);
					//serialWrite_newline();
					//serialWrite_uint_data((unsigned long)(New_Location.Longtitude*100000),data_long);
					//serialWrite_newline();

					//				PORTD^=(1<<PD7);

					sample_no++;
				}
			}
			if (sample_no >= 10)
			{
				sample_no = 0;

				Location_Sum.Latitude /= 10;
				Location_Sum.Longtitude /= 10;
				Location_Sum.Altitude /= 10;

				gps_distance_t = nmea_Calculate_distance(Old_Location, Location_Sum); //Sum gps_distance

				if (gps_distance_t > 0.5)
				{
					gps_distance += gps_distance_t;
				}

				serialWrite_double(gps_distance, 3);
				serialWrite_newline();

				Old_Location.Latitude = Location_Sum.Latitude;
				Old_Location.Longtitude = Location_Sum.Longtitude;
				Old_Location.Altitude = Location_Sum.Altitude;

				Location_Sum.Latitude = 0;
				Location_Sum.Longtitude = 0;
				Location_Sum.Altitude = 10;

				if (init_log == 0)
				{
					gps_distance = -1;
					init_log = 1;
				}
			}

			//serialWrite_uint_data((unsigned long)(gps_distance*1000),data_gps_distance);
			//serialWrite_uint_data((unsigned long)(mpu_distance*1000),data_acc_distance);

			_delay_us(50);
			if (SPI_FLAG == 1)
			{
				mpu_distance = SPI_read_double();
				SPI_FLAG = 0;
				serialWrite("GPS: ");
				serialWrite_double(gps_distance, 3);
				serialWrite("   ");
				serialWrite("ACC: ");
				serialWrite_double(mpu_distance, 5);
				serialWrite_newline();
				lcd_cmd(clr_display);
				lcd_cursor_pos(1, 1);
				lcd_write("GPS: ");
				lcd_write_double(gps_distance, 3);
				lcd_cursor_pos(1, 2);
				lcd_write("ACC: ");
				lcd_write_double(mpu_distance, 3);
			}
		}

		if (SPI_FLAG == 1)
		{
			mpu_distance = SPI_read_double();
			SPI_FLAG = 0;
		}
		// 			serialWrite("GPS gps_distance: ");
		// 			serialWrite_double(gps_distance,3);
		// 			serialWrite("   ");
		// 			serialWrite("MPU gps_distance: ");
		// 			serialWrite_double(mpu_distance,5);
		// 			serialWrite_newline();
		lcd_cmd(clr_display);
		lcd_cursor_pos(1, 1);
		lcd_write("GPS: ");
		lcd_write_double(gps_distance, 3);
		lcd_cursor_pos(1, 2);
		lcd_write("ACC: ");
		lcd_write_double(mpu_distance, 3);

		//	PORTB^=(1<<PINB0);
		// 	_delay_us(50);
		// 	PORTB^=(1<<PINB0);
	}
}

ISR(INT1_vect)
{
	_delay_us(500);
	if (log_button)
	{
		log_button = 0;
	}
	else
	{
		log_button = 1;
	}
}