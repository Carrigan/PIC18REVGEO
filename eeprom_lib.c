#include <p18f26k22.h>
#include "eeprom_lib.h"

unsigned char ee_read(unsigned char address)
{
	EEADRH = 0;
	EEADR = address;

	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;

	EECON1bits.RD = 1;

	return EEDATA;
} 

void ee_write(unsigned char x, unsigned char address)
{
	EECON1bits.WREN = 1;
	INTCONbits.GIEH = 0;
	
	// Write data
	EEADRH = 0;
	EEADR = address;
	EECON1bits.EEPGD = 0;
	EECON1bits.CFGS = 0;
	EEDATA = x;

	// Write sequence
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;

	// Delay until write is done
	while(EECON1bits.WR != 0);
	INTCONbits.GIEH = 1;
	EECON1bits.WREN = 0;
}
