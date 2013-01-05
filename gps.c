#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gps.h"
#include "LCD_lib.h"

// Comma Meaning
#define COMMA_TIME	0
#define COMMA_LAT	1
#define COMMA_LONG	3
#define COMMA_FIX	5
#define COMMA_SATS	6

// Target Coordinates
#define TARGET_LAT	ADD_YOUR_OWN!
#define TARGET_LON	ADD_YOUR_OWN!

// Main Data Structure
gps_data_t gps;

// Ping Pong Buffers
char BUFFER0[70];
char BUFFER1[70];

// GPS Output Buffer
char buffer[16];

// Buffer Helpers
char currentBuffer;
char isDataAvailable;
char *currentWrite;
char *dataStart;

const char gpgga[] = "$GPGGA";

void gps_init(void)
{
	currentBuffer = 0;
	isDataAvailable = 0;
	currentWrite = BUFFER0;
	dataStart = BUFFER1;

	gps.fix = 0;
}

void gps_receiveCharacter(char input)
{
	if(input != '\n')
	{
		*currentWrite = input;
		currentWrite++;
	} else {
		// Terminate the string
		currentWrite = 0;

		// Swap the buffer
		isDataAvailable = 1;
		currentWrite = dataStart;
		dataStart = (currentBuffer == 0) ? BUFFER0 : BUFFER1;
		currentBuffer = (currentBuffer + 1) % 2;
	}
}


void gps_parse(void)
{
	unsigned char commas[14];
	if(isDataAvailable)
	{
		isDataAvailable = 0;

		// Check if the last line received was a GPGGA string
		if(strstr(dataStart, gpgga) == NULL)
		{
		} else {
			unsigned long temp = 0;
			unsigned char index = 0;
			float tempMath;
			char *currentRead;

			// Find the locations of all commas, replace with 0s:
			currentRead = dataStart;
			while(*currentRead != 0)
			{
				if(*currentRead == ',')
				{
					commas[temp] = index;
					*currentRead = 0;
					temp++;
				}
				index++;
				currentRead++;
			}

			// Check if the information is valid. If so - heavy lifting.
			if(atoi(dataStart + 1 + commas[COMMA_FIX]) == 1)
			{
				gps.fix = 1;
				// Produce usable GPS Data
				// Compute Time:
				temp = atol(dataStart + 1 + commas[COMMA_TIME]);
				gps.hours = temp / 10000;
				gps.minutes = (temp / 100) % 100;
				gps.seconds = temp % 100;

				// Compute Latitude, in exact degrees:
				tempMath = atof(dataStart + 3 + commas[COMMA_LAT]);
				tempMath = tempMath /60.0;
				gps.latitude = ((atoi(dataStart + 1 + commas[COMMA_LAT]) / 100) + tempMath);

				// Compute Longitude, in exact degrees:
				tempMath = atof(dataStart + 4 + commas[COMMA_LONG]);
				tempMath = tempMath /60.0;
				gps.longitude = ((atoi(dataStart + 1 + commas[COMMA_LONG]) / 100) + tempMath);
			}	// GPS Fix
			else
				gps.fix = 0;
		}		// GPGGA String
	}			// Data Available
}		


float gps_computeDistance(void)
{
	char dmessage[] = "Distance:";
	// Convert data strings to coordinates. Must be exact degrees.
	float myNorthing = gps.latitude;
	float myEasting = gps.longitude;
	float pyth_lat = fabs(TARGET_LAT - gps.latitude)*69.172;
	float pyth_lon = fabs(TARGET_LON - gps.longitude)*18.485;
	float distance;

	distance = sqrt((pyth_lat * pyth_lat) + (pyth_lon * pyth_lon));
	
	return distance;
}


gps_data_t *gps_getPointer(void)
{
	return &gps;
}
