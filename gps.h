#ifndef GPS_H
#define GPS_H


typedef struct
{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
	double latitude;
	double longitude;
	unsigned char fix;
} gps_data_t;

void 		gps_init(void);
void 		gps_receiveCharacter(char input);
void 		gps_parse(void);
gps_data_t	*gps_getPointer(void);
float 		gps_computeDistance(void);

#endif
