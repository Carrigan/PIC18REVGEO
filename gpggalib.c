/**
 *	
 *	@author Brian Carrigan
 *	@date	8/3/2013
 *	
 *	@brief NMEA GPGGA state based parser.
 *	
 *	Future improvements:
 *		- Change all double operations to integers.
 */

#include "gpggalib.h"

//! A macro for the GPS data coming in.
#define	CURRENT_GPS	mGpsDataPingPong[mGpsPingPongIndex]

//! A function pointer definition for state machine functions.
typedef GpsReturn_t (*CharacterProcessor_t)(uint8_t);

static GpsReturn_t GPS_IDLE(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_$(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_G1(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_P(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_G2(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_G3(uint8_t pDataIn);
static GpsReturn_t GPS_RCVD_A(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_TIME(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_LATITUDE(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_LATITUDE_DIR(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_LONGITUDE(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_LONGITUDE_DIR(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_FIX(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_SATELLITE_CNT(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_DILUTION(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_ALTITUDE(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_ALTITUDE_UNIT(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_GEOID(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_GEOID_UNIT(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_TRASH(uint8_t pDataIn);
static GpsReturn_t GPS_RCVING_CHECKSUM(uint8_t pDataIn);

static uint8_t	charToInt(uint8_t pDataIn);
static uint8_t	hexToInt(uint8_t pDataIn);

static CharacterProcessor_t mCurrentState;
static GpsData_t mGpsDataPingPong[2];
static uint8_t mGpsPingPongIndex;
static uint8_t mGpsLastRcvd;
static uint8_t mGpsChecksum;
static uint8_t mLocalCounter;

/**
 *	@brief Initialize the GPGGA library.
 *
 *	Initializes the state machine, buffers, and sets the valid bytes to 0.
 */
void gpgga_init(void)
{
	mGpsLastRcvd = 1;
	mGpsPingPongIndex = 0;
	mCurrentState = GPS_IDLE;
	mGpsDataPingPong[0].valid = 0;
	mGpsDataPingPong[1].valid = 0;
}

/**
 * 	@brief 		Feed a single character to the GPGGA state machine.
 *	
 *	
 *	@return 	Returns GPS_DATA_RDY when a full GPS GPGGA string has been parsed
 *				and the checksum verified, GPS_NO_DATA otherwise.
 */
GpsReturn_t		gpgga_feedCharacter(uint8_t pDataIn)
{
	// If it was a $, reset everything.
	if(pDataIn == '$')
		mCurrentState = GPS_IDLE;

	// Continue the checksum computation.
	mGpsChecksum ^= pDataIn;

	// Feed it to the appropriate handler.
	return mCurrentState(pDataIn);
}

/**
 *	@brief Returns a pointer to the most recently completed GPS datum.
 *	
 *	The data can be checked for validity by checking the valid byte.
 */
GpsData_t		*gpgga_getLastGpsData()
{
	return &(mGpsDataPingPong[mGpsLastRcvd]);
}



/******************************************************************************
 *                            Local Functions                                 *
 *****************************************************************************/

static uint8_t	isNumericCharacter(uint8_t pDataIn)
{
	if(pDataIn == '.' || ( (pDataIn >= 48) && (pDataIn <= 57)))
	{
		return 1;
	} else {
		return 0;
	}
}

static uint8_t	charToInt(uint8_t pDataIn)
{
	// '0' to 0, '1' to 1, etc..
	return pDataIn - 48;
}

static uint8_t	hexToInt(uint8_t pDataIn)
{
	if( (pDataIn >= 65) && (pDataIn <= 70) )
		return pDataIn - 55;
	else if( (pDataIn >= 97) && (pDataIn <= 102) )
		return pDataIn - 87;
	else
		return pDataIn - 48;
}


static GpsReturn_t GPS_IDLE(uint8_t pDataIn)
{
	if(pDataIn == '$')
	{
		mCurrentState = GPS_RCVD_$;
		mGpsChecksum = 0;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_$(uint8_t pDataIn)
{
	if(pDataIn == 'G')
	{
		mCurrentState = GPS_RCVD_G1;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_G1(uint8_t pDataIn)
{
	if(pDataIn == 'P')
	{
		mCurrentState = GPS_RCVD_P;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_P(uint8_t pDataIn)
{
	if(pDataIn == 'G')
	{
		mCurrentState = GPS_RCVD_G2;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_G2(uint8_t pDataIn)
{
	if(pDataIn == 'G')
	{
		mCurrentState = GPS_RCVD_G3;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_G3(uint8_t pDataIn)
{
	if(pDataIn == 'A')
	{
		mCurrentState = GPS_RCVD_A;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVD_A(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mLocalCounter = 0;
		mCurrentState = GPS_RCVING_TIME;
	} else {
		mCurrentState = GPS_IDLE;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_TIME(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mLocalCounter = 0;
		mCurrentState = GPS_RCVING_LATITUDE;
	} else {
		if(mLocalCounter == 0)
		{
			CURRENT_GPS.hours = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 1) {
			CURRENT_GPS.hours += charToInt(pDataIn);
		} else if (mLocalCounter == 2) {
			CURRENT_GPS.minutes = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 3) {
			CURRENT_GPS.minutes += charToInt(pDataIn);
		} else if (mLocalCounter == 4) {
			CURRENT_GPS.seconds = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 5) {
			CURRENT_GPS.seconds += charToInt(pDataIn);
		}
		mLocalCounter++;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_LATITUDE(uint8_t pDataIn)
{
	static uint32_t	tempMath1;
	static uint32_t	tempMath2;
	static uint32_t tempMath3;
	static uint32_t tempMath4;

	if(pDataIn == ',')
	{
		// Calculate the latitude. mLocalCounter is used to see how many precision decimals there are.
		CURRENT_GPS.latitude = tempMath1 + tempMath2/60.0 + (float)tempMath3/tempMath4;
		mLocalCounter = 0;
		mCurrentState = GPS_RCVING_LATITUDE_DIR;
	} else {
		if(mLocalCounter == 0)
		{
			tempMath1 = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 1) {
			tempMath1 += charToInt(pDataIn);
		} else if (mLocalCounter == 2) {
			tempMath2 = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 3) {
			tempMath2 += charToInt(pDataIn);
			tempMath3 = 0;
			tempMath4 = 60;
		} else if (mLocalCounter > 4) {
			tempMath3 = tempMath3 * 10 + charToInt(pDataIn);
			tempMath4 *= 10;
		}
		mLocalCounter++;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_LATITUDE_DIR(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_LONGITUDE;
	} else if (pDataIn == 'S') {
		CURRENT_GPS.latitude *= -1;
	} 
	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_LONGITUDE(uint8_t pDataIn)
{
	static uint32_t	tempMath1;
	static uint32_t	tempMath2;
	static uint32_t tempMath3;
	static uint32_t tempMath4;

	if(pDataIn == ',')
	{
		CURRENT_GPS.longitude = tempMath1 + tempMath2/60.0 + tempMath3/(float)tempMath4;
		tempMath1 = 0;
		tempMath2 = 0;
		tempMath3 = 0;
		mLocalCounter = 0;
		mCurrentState = GPS_RCVING_LONGITUDE_DIR;
	} else {
		if(mLocalCounter == 0)
		{
			tempMath1 = charToInt(pDataIn) * 100;
		} else if (mLocalCounter == 1) {
			tempMath1 += charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 2) {
			tempMath1 += charToInt(pDataIn);
		} else if (mLocalCounter == 3) {
			tempMath2 = charToInt(pDataIn) * 10;
		} else if (mLocalCounter == 4) {
			tempMath2 += charToInt(pDataIn);
			tempMath3 = 0;
			tempMath4 = 60;
		} else if (mLocalCounter > 5) {
			tempMath3 = tempMath3 * 10 + charToInt(pDataIn);
			tempMath4 *= 10;
		}

		mLocalCounter++;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_LONGITUDE_DIR(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_FIX;
	} else if (pDataIn == 'W') {
		CURRENT_GPS.longitude *= -1;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_FIX(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_SATELLITE_CNT;
	} else {
		CURRENT_GPS.fix = (FixQuality_t)charToInt(pDataIn);
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_SATELLITE_CNT(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_DILUTION;
		mLocalCounter = 0;
	} else {
		if(mLocalCounter == 0)
		{
			CURRENT_GPS.satelliteCount = charToInt(pDataIn) * 10;
			mLocalCounter++;
		} else {
			CURRENT_GPS.satelliteCount += charToInt(pDataIn);
		}
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_DILUTION(uint8_t pDataIn)
{
	static uint32_t tempMath1 = 0;

	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_ALTITUDE;
		CURRENT_GPS.horizontalDilution = tempMath1;
		tempMath1 = 0;
	} else {
		if(pDataIn != '.')
		{
			tempMath1 = tempMath1 * 10 + charToInt(pDataIn);
		}
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_ALTITUDE(uint8_t pDataIn)
{
	static uint32_t tempMath1 = 0;

	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_ALTITUDE_UNIT;
		CURRENT_GPS.altitude = tempMath1;
		tempMath1 = 0;
	} else {
		if(pDataIn != '.')
		{
			tempMath1 = tempMath1 * 10 + charToInt(pDataIn);
		}
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_ALTITUDE_UNIT(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_GEOID;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_GEOID(uint8_t pDataIn)
{
	static uint32_t tempMath1 = 0;
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_GEOID_UNIT;
		CURRENT_GPS.geoidHeight = tempMath1;
		tempMath1 = 0;
	} else {
		if(pDataIn != '.')
		{
			tempMath1 = tempMath1 * 10 + charToInt(pDataIn);
		}
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_GEOID_UNIT(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_TRASH;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_TRASH(uint8_t pDataIn)
{
	if(pDataIn == ',')
	{
		mCurrentState = GPS_RCVING_CHECKSUM;
		CURRENT_GPS.checksum = mGpsChecksum;
	}

	return GPS_NO_DATA;
}

static GpsReturn_t GPS_RCVING_CHECKSUM(uint8_t pDataIn)
{
	static uint32_t tempMath1;

	if(mLocalCounter == 0)
	{
		if(pDataIn != '*')
		{
			mCurrentState = GPS_IDLE;
		}
	}
	else if(mLocalCounter == 1)
	{
		tempMath1 = hexToInt(pDataIn) * 16;
	}
	else if(mLocalCounter == 2)
	{
		tempMath1 += hexToInt(pDataIn);
	}
	else if(mLocalCounter == 3)
	{
		// We're done here. Set valid if the checksums meet.
		if(tempMath1 == CURRENT_GPS.checksum)
		{
			CURRENT_GPS.valid = 1;
			
			// Flip the buffer:
			if(mGpsPingPongIndex == 0)
			{
				mGpsPingPongIndex = 1;
				mGpsLastRcvd = 0;
			} else {
				mGpsPingPongIndex = 0;
				mGpsLastRcvd = 1;
			}

			return GPS_DATA_RDY;
		}

		mCurrentState = GPS_IDLE;
	}

	mLocalCounter++;
	return GPS_NO_DATA;
}
