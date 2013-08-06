#include "gpsmath.h"

/** 
 * 	Quick and dirty distance computation that works for NJ without having to use
 *	any sines/cosines to compute great arcs.
 */
float gpsmath_computeDistance(GpsData_t *pGpsData, float pTargetNorthing, float pTargetEasting)
{
	// Convert data strings to coordinates. Must be exact degrees.
	float pyth_lat = fabs(pTargetNorthing - pGpsData->latitude)*69.172;
	float pyth_lon = fabs(pTargetEasting - pGpsData->longitude)*18.485;
	float distance;

	distance = sqrt((pyth_lat * pyth_lat) + (pyth_lon * pyth_lon));
	
	return distance;
}