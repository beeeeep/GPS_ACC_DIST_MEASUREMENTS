#include "GPS_nemea.h"

void GPS_GET_GPGGA_data(GPGGA_data *GGA_packet)
{
	uint8_t i = 6;
	uint8_t pointer = 0;
	char *Altitude;
	char *nemea_packet;
	char GPSTime[5];
	char *Speed;
	char *Heading;
	char *GPSDate;
	do
	{
		nemea_packet = serialRead_nemea_string();
	} while (!strncmp(nemea_packet, "GPGGA", 5));

	//start parsing just after "GPGGA,"

	// attempt to reject empty packets right away
	if ((nemea_packet[i] == ',') & (nemea_packet[i + 1] == ','))
		return;

	// get UTC time [hhmmss.sss]

	while (nemea_packet[i] != ',') // next field: Time
	{
		GPSTime[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field: Latitude
	{
		//do nothing
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field: N/S
	{
		//do nothing
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field: Longitude
	{
		//do nothing
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field: E/W
	{
		//do nothing
		i++;
		pointer++;
	}
	i++;
	pointer = 0;

	while (nemea_packet[i] != ',') // next field:Fix Quality
	{
		GGA_packet->fix_quality = atoi(nemea_packet[i]);
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:No of satellites
	{
		//bypass no of satellites
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:horizontal dilution of position
	{
		//bypass horizontal dilution of position
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:Altitude in meters above sea level
	{
		Altitude[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}
}

void GPS_GET_GPRMC_data(GPRMC_data *RMC_packet)
{
	uint8_t i;
	uint8_t pointer = 0;
	char *nemea_packet;
	char GPSTime[5];
	char *Lat;
	char *Long;
	char *Speed;
	char Char_buffer[2];
	char *GPSDate;
	char *Heading;
	do
	{
		nemea_packet = serialRead_nemea_string();
	} while (!strncmp(nemea_packet, "GPRMC", 5));

	//start parsing just after "GPRMC,"
	i = 6;
	// attempt to reject empty packets right away
	if ((nemea_packet[i] == ',') & (nemea_packet[i + 1] == ','))
		return;

	while (nemea_packet[i] != ',') // next field: Time
	{
		GPSTime[pointer] = nemea_packet[i];
		i++;
		pointer++;
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	serialWrite("Test");
	serialWrite_newline();
	//Convert Hours
	Char_buffer[0] = GPSTime[0];
	Char_buffer[1] = GPSTime[1];
	RMC_packet->Hour = atoi(Char_buffer);

	//Convert Mins
	Char_buffer[0] = GPSTime[2];
	Char_buffer[1] = GPSTime[3];
	RMC_packet->Min = atoi(Char_buffer);

	//Conver Secs
	Char_buffer[0] = GPSTime[4];
	Char_buffer[1] = GPSTime[5];
	RMC_packet->Sec = atoi(Char_buffer);

	while (nemea_packet[i] != ',') // next field: Navigation warning
	{
		//do nothing
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:Latitude
	{
		Lat[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}

	RMC_packet->Latitude = atof(Lat);

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:N/S
	{
		RMC_packet->Latitude_dir = nemea_packet[i];
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:Longitude
	{
		Long[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}
	RMC_packet->Longitude = atof(Long);
	i++;
	pointer = 0;
	while (nemea_packet[i] != ',') // next field:E/W
	{
		RMC_packet->Longitude_dir = nemea_packet[i];
		i++;
		pointer++;
	}

	i++;
	pointer = 0;
	// get speed
	while (nemea_packet[i] != ',') // next field: speed
	{
		Speed[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	RMC_packet->Speed = atof(Speed);
	while (nemea_packet[i] != ',') // next field:Course made good
	{
		Heading[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}
	i++;
	pointer = 0;
	RMC_packet->Heading = atof(Heading);

	while (nemea_packet[i] != ',') // next field:Date
	{
		GPSDate[pointer] = nemea_packet[i];
		i++;
		pointer++;
	}

	// Convert Days
	Char_buffer[0] = GPSDate[0];
	Char_buffer[1] = GPSDate[1];
	RMC_packet->Day = atoi(Char_buffer);

	// Convert Month
	Char_buffer[0] = GPSDate[2];
	Char_buffer[1] = GPSDate[3];
	RMC_packet->Month = atoi(Char_buffer);

	// Convert Year
	Char_buffer[0] = GPSDate[4];
	Char_buffer[1] = GPSDate[5];
	RMC_packet->Year = atoi(Char_buffer);
}
