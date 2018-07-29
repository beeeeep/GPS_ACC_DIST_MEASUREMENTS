/*! \file nmea.h \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.h'
// Title		: NMEA protocol function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
///	\ingroup driver_hw
/// \defgroup nmea NMEA Packet Interface for GPS Receivers (nmea.c)
/// \code #include "nmea.h" \endcode
/// \par Overview
///		This library parses and decodes the standard NMEA data stream from a
/// GPS and stores the position, velocity, and time solutions in the gps.c
/// library.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef NMEA_H
#define NMEA_H

#include "txSerial.h"
// constants/macros/typdefs
#define NMEA_BUFFERSIZE 80

// Message Codes
#define NMEA_NODATA 0	 // No data. Packet not available, bad, or not decoded
#define NMEA_GPGGA 1	  // Global Positioning System Fix Data
#define NMEA_GPVTG 2	  // Course over ground and ground speed
#define NMEA_GPGLL 3	  // Geographic position - latitude/longitude
#define NMEA_GPGSV 4	  // GPS satellites in view
#define NMEA_GPGSA 5	  // GPS DOP and active satellites
#define NMEA_GPRMC 6	  // Recommended minimum specific GPS data
#define NMEA_UNKNOWN 0xFF // Packet received but not known
#define Earth_Radius 6371000

char GPSTime[6];
char Lat[9];
char Long[11];
char NS[1];
char EW[1];
char FixQuality[4];
char Altitude[7];
char No_of_sats[3];

// GPRMC data
char Speed[5]; //in knots
char Heading[5];
char GPSDate[6];

typedef struct
{
	double Latitude;
	double Longtitude;
	char Latidude_dir;
	char Longtitude_dir;
	int Altitude;
} struct_Location;

// Debugging
//#define NMEA_DEBUG_PKT	///< define to enable debug of all NMEA messages
//#define NMEA_DEBUG_GGA	///< define to enable debug of GGA messages
//#define NMEA_DEBUG_VTG	///< define to enable debug of VTG messages

// functions

uint8_t nmeaProcess(void);
void nmeaProcessGPGGA(char *packet);
void nmeaProcessGPGSV(char *packet);
void nmeaProcessGPRMC(char *packet);
void nmea_Normalise_Data(char *Lat, char *Long, char *NS, char *EW, char *Altitude, struct_Location *Location);
float nmea_Calculate_distance(struct_Location Start_Location, struct_Location End_Location);

#endif
