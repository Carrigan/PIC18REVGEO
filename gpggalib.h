/**
 *	
 *	@author Brian Carrigan
 *	@date	8/3/2013
 *	
 *	@brief NMEA GPGGA state based parser.
 *	
 *	Intended use example:
 *	
 *	void main()
 *	{
 *		GpsData_t *myGpsData;
 *		
 *		while(1)
 *		{
 *			if(serialDataReady())
 *			{
 *				GpsReturn_t dataReady = gpgga_feedCharacter(Uart In);
 *				if(dataReady)
 *				{
 *					myGpsData = gpgga_getLastGpsData();
 *					
 *					// Process data here
 *					printf("LAT: %f\n LON: %f\n", myGpsData->latitude, myGpsData->longitude);
 *				}
 *			}
 *		}
 *	}
 */

#ifndef GPGGALIB
#define GPGGALIB

#include <stdint.h>

//! Enum returned by the state machine to denote if data is ready.
typedef enum 
{  
	GPS_NO_DATA		= 0,
	GPS_DATA_RDY	= 1
} GpsReturn_t;

//!	The quality of the GPS Fix, as defined by NMEA standards.
typedef enum 
{  
	FIX_INVALID 	= 0,
	FIX_GPS 		= 1,
	FIX_DGPS		= 2,
	FIX_PPS			= 3,
	FIX_RTK 		= 4,
	FIX_FLOATRTK	= 5,
	FIX_ESTIMATED 	= 6,
	FIX_MANUAL		= 7,
	FIX_SIM 		= 8
} FixQuality_t;

//! Struct that is returned by the library. 21 bytes packed.
typedef struct 
{
	uint8_t 		hours;
	uint8_t 		minutes;
	uint8_t 		seconds;
	float 			latitude;
	float 			longitude;
	FixQuality_t 	fix;
	uint8_t			satelliteCount;
	uint8_t			horizontalDilution;
	uint16_t		altitude;
	uint16_t		geoidHeight;
	uint16_t		checksum;
	uint8_t			valid;
} GpsData_t;

void			gpgga_init(void);
GpsReturn_t		gpgga_feedCharacter(uint8_t pDataIn);
GpsData_t		*gpgga_getLastGpsData();

#endif
