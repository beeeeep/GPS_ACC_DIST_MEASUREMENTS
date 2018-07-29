#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "nmea.h"

// Global variables
extern uint8_t status;
char *NMEAPacket;
extern char GPGGA[80];
double pi = 3.141592653589793238462643;

uint8_t nmeaProcess(void) //(cBuffer* rxBuffer)
{
	uint8_t foundpacket = NMEA_NODATA;

	NMEAPacket = serialRead_nemea_string();

	// check message type and process appropriately
	if (!strncmp(NMEAPacket, "GPGGA", 5))
	{
		//	 process packet of this type
		//  			serialWrite(NMEAPacket);
		// 		serialWrite_newline();
		nmeaProcessGPGGA(NMEAPacket);
		// report packet type
		foundpacket = NMEA_GPGGA;
	}

	else if (!strncmp(NMEAPacket, "GPRMC", 5))
	{
		//  			serialWrite(NMEAPacket);
		//  			serialWrite_newline();
		// process packet of this type
		nmeaProcessGPRMC(NMEAPacket);
		// report packet type
		foundpacket = NMEA_GPRMC;
	}

	return foundpacket;
}

void nmeaProcessGPGGA(char *packet)
{

	uint8_t i;
	uint8_t GPIndex = 0;

	//start parsing just after "GPGGA,"
	i = 7;
	// attempt to reject empty packets right away

	if ((packet[i] == ',') & (packet[i + 1] == ','))
		return;

	// get UTC time [hhmmss.sss]
	while (packet[i] != ',') // next field: Time
	{
		GPSTime[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field: Latitude
	{
		Lat[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field: N/S
	{
		NS[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field: Longitude
	{
		Long[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field: E/W
	{
		EW[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;

	while (packet[i] != ',') // next field:Fix Quality
	{
		FixQuality[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:No of satellites
	{

		No_of_sats[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:horizontal dilution of position
	{
		//bypass horizontal dilution of position
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:Altitude in meters above sea level
	{
		Altitude[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
	//GPIndex=0;
	// next field: longitude					//this method can be used to access a variable directly
	//for(int i=25;i<35;i++)
	//{
	//Long[GPIndex]=packet[i];
	//GPIndex++;
	//}
	// get latitude [ddmm.mmmmm]
	//latitude = strtod(&packet[i], &endptr);

	//minutesfrac = modf(latitude/100, &degrees);
	//latitude= degrees + (minutesfrac*100)/60;
	//latitude *= (pi/180);
}

void nmeaProcessGPGSV(char *packet)
{
}

void nmeaProcessGPRMC(char *packet)
{
	uint8_t i;
	uint8_t GPIndex = 0;

	//start parsing just after "GPRMC,"
	i = 6;
	// attempt to reject empty packets right away
	if ((packet[i] == ',') & (packet[i + 1] == ','))
		return;

	while (packet[i] != ',') // next field: Time
	{
		//do nothing
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field: Navigation warning
	{
		//do nothing
		i++;
		GPIndex++;
	}
	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:Latitude
	{
		Lat[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:N/S
	{
		NS[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:Longitude
	{
		Long[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:E/W
	{
		EW[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	// get speed
	while (packet[i] != ',') // next field: speed
	{
		Speed[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:Course made good
	{
		Heading[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}

	i++;
	GPIndex = 0;
	while (packet[i] != ',') // next field:Date
	{
		GPSDate[GPIndex] = packet[i];
		i++;
		GPIndex++;
	}
}

void nmea_Normalise_Data(char *Lat, char *Long, char *NS, char *EW, char *Altitude, struct_Location *Location)
{
	int degrees;

	Location->Latitude = strtod(Lat, NULL) * 0.01;
	degrees = Location->Latitude;
	Location->Latitude = (100 * (Location->Latitude - degrees) / 60) + degrees;

	Location->Longtitude = strtod(Long, NULL) * 0.01;
	degrees = Location->Longtitude;
	Location->Longtitude = (100 * (Location->Longtitude - degrees) / 60) + degrees;

	Location->Latidude_dir = EW[0];
	Location->Longtitude_dir = NS[0];

	Location->Altitude = atoi(Altitude);
}

float nmea_Calculate_distance(struct_Location Start_Location, struct_Location End_Location)
{
	double Lat_Start_Rads;
	double Lat_End_Rads;
	double Long_Start_Rads;
	double Long_End_Rads;
	double a;
	double c;
	double d;
	double DLat_sin_sqr;
	double DLong_sin_sqr;

	Lat_Start_Rads = Start_Location.Latitude * pi / 180;
	Long_Start_Rads = Start_Location.Longtitude * pi / 180;

	Lat_End_Rads = End_Location.Latitude * pi / 180;
	Long_End_Rads = End_Location.Longtitude * pi / 180;

	DLat_sin_sqr = sin((Lat_End_Rads - Lat_Start_Rads) / 2);
	DLat_sin_sqr *= DLat_sin_sqr;
	DLong_sin_sqr = sin((Long_End_Rads - Long_Start_Rads) / 2);
	DLong_sin_sqr *= DLong_sin_sqr;
	a = DLat_sin_sqr + cos(Lat_Start_Rads) * cos(Lat_End_Rads) * DLong_sin_sqr;

	c = 2 * atan2(sqrt(a), sqrt(1 - a));

	d = c * Earth_Radius;

	d = sqrt(d * d + (End_Location.Altitude - Start_Location.Altitude) * (End_Location.Altitude - Start_Location.Altitude));

	return (d);
}
