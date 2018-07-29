#ifndef GPS_NEMEA_H_
#define GPS_NEMEA_H_

#include "txSerial.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef struct
{
	uint8_t Thours;
	uint8_t Tmins;
	uint8_t Tsecs;
	double Latitude;
	char Latitude_dir;
	double Longitude;
	char Longitute_dir;
	int8_t fix_quality;
	uint8_t num_of_sat;
	uint8_t Altitude; //meters,above sea level
} GPGGA_data;

typedef struct
{
	double Latitude;
	double Longitude;
	char Latitude_dir;
	char Longitude_dir;
	float Speed;
	float Heading;
	int8_t Day;
	int8_t Month;
	int8_t Year;
	int8_t Hour;
	int8_t Min;
	int8_t Sec;
}
GPRMC_data;

void GPS_GET_GPGGA_data(GPGGA_data *GGA_packet);
void GPS_GET_GPRMC_data(GPRMC_data *RMC_packet);






#endif /* GPS_NEMEA_H_ */