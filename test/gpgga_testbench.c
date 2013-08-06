#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gpggalib.h"

#define TEST_DATA_SIZE	(sizeof(gpsTestData) / sizeof(uint8_t))

int compareGpsDatum(GpsData_t *dataOne, GpsData_t *dataTwo);

int main()
{
	int i;

	uint8_t testsFailed = 0;

	// Number of strings found
	uint8_t positiveCount = 0;

	// Test String:
	uint8_t gpsTestData[] = "$GPGGA,123518,4807.033~corruptionhere~$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n$GPGGA,123161,\n";

	// Received Data:
	GpsData_t *receivedData;

	// Expected Output 1:
	GpsData_t expectedData;
	expectedData.hours = 12;
	expectedData.minutes = 35;
	expectedData.seconds = 19;
	expectedData.latitude = 48 + (7.038 / 60.0);
	expectedData.longitude = 11 + (31.000 / 60.0);
	expectedData.fix = FIX_GPS;
	expectedData.satelliteCount = 8;
	expectedData.horizontalDilution = 9;
	expectedData.altitude = 5454;
	expectedData.geoidHeight = 469;
	expectedData.checksum = 71;
	expectedData.valid = 1;

	// Feed it
	printf("Feeding the following string:\n%s", gpsTestData);
	for(i = 0; i < TEST_DATA_SIZE; i++)
		positiveCount += gpgga_feedCharacter(gpsTestData[i]);

	// Check the results:
	receivedData = gpgga_getLastGpsData();
	compareGpsDatum(receivedData, &expectedData);
	
	while(1);
	return 0;
}

int compareGpsDatum(GpsData_t *dataOne, GpsData_t *dataTwo)
{
	const uint8_t passString[] =	"pass";
	const uint8_t failString[] =	"fail";

	printf("\nResults:\n");
	printf("Hours:\t\t\t%s\n", (dataOne->hours == dataTwo->hours) ? passString : failString );
	printf("Minutes:\t\t%s\n", (dataOne->minutes == dataTwo->minutes) ? passString : failString);
	printf("Seconds:\t\t%s\n", (dataOne->seconds == dataTwo->seconds) ? passString : failString);
	printf("Latitude:\t\t%s\n", (dataOne->latitude == dataTwo->latitude) ? passString : failString);
	printf("Longitude:\t\t%s\n", (dataOne->longitude == dataTwo->longitude) ? passString : failString);
	printf("Fix Quality:\t\t%s\n", (dataOne->fix == dataTwo->fix) ? passString : failString);
	printf("Satellite Count:\t%s\n", (dataOne->satelliteCount == dataTwo->satelliteCount) ? passString : failString);
	printf("Dilution:\t\t%s\n", (dataOne->horizontalDilution == dataTwo->horizontalDilution) ? passString : failString);
	printf("Altitude:\t\t%s\n", (dataOne->altitude == dataTwo->altitude) ? passString : failString);
	printf("Height of GeoID:\t%s\n", (dataOne->geoidHeight == dataTwo->geoidHeight) ? passString : failString);
	printf("Checksum:\t\t%s\n", (dataOne->checksum == dataTwo->checksum) ? passString : failString);
	printf("Valid data:\t\t%s\n", (dataOne->valid == dataTwo->valid) ? passString : failString);
	printf("\n");

	return 0;
}


