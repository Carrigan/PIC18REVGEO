#include <p18f26k22.h>
#include <string.h>
#include "LCD_lib.h"

#define E_PIN LATCbits.LATC3
#define E_TRIS TRISCbits.TRISC3

#define RW_PIN LATCbits.LATC2
#define RW_TRIS TRISCbits.TRISC2

#define RS_PIN LATCbits.LATC1
#define RS_TRIS TRISCbits.TRISC1

#define OUTPORT LATA
#define BUSY_IN PORTAbits.RA3
#define OUTPORT_TRIS TRISA

void command(unsigned char i);
void toggleClock(void);
void DELAY(int MS);
void usDELAY(unsigned int uS);

void command(unsigned char i)
{
	OUTPORT = (i>>4) & 0x0F;
	RS_PIN = 0;
	RW_PIN = 0;
	toggleClock();
	OUTPORT = i & 0x0F;
	toggleClock();	
}

void toggleClock()
{
	E_PIN = 1;
	Nop();
	E_PIN = 0;
}

void DELAY(int MS)
{
	// At 4MHZ: 1 ms = 1k cycles:
	// MAX = 25ms
	int outer;
	int i;
	for(outer = 0; outer < MS; outer++)
		for (i = 0; i < 1000; i++)
			;
}

void usDELAY(unsigned int uS)
{
	int i;
	for(i = 0; i < uS; i++)
		;
}

// Goes to character 0, line x
void lcd_setLine(unsigned char line)
{
	const unsigned char lineIDs[5] = {0, 0x80, 0xC0, 0x90, 0xD0};
	command(lineIDs[line]);
	lcd_waitUntilReady();
}

#define DISPLAY_ON 0x0F
#define DISPLAY_OFF 0b00001011
#define CURSOR_ON 0x0F
#define CURSOR_OFF 0b00001101
#define BLINK_ON 0x0F
#define BLINK_OFF 0b00001110
void lcd_setDisplay(unsigned char config)
{
	command(config);
	lcd_waitUntilReady();
}

#define ID_RIGHT 	0b00000111
#define ID_LEFT		0b00000101
#define SHIFT_ON	0b00000111 
#define SHIFT_OFF	0b00000110 

void lcd_setEntry(unsigned char config)
{
	command(config);
	lcd_waitUntilReady();
}

void lcd_sendData(unsigned char i)
{
	OUTPORT = (i>>4) & 0x0F;
	RS_PIN = 1;
	RW_PIN = 0;
	toggleClock();
	OUTPORT = i & 0x0F;
	toggleClock();
	lcd_waitUntilReady();	
}

void lcd_waitUntilReady(void)
{
	char breakchar = 0;
	// Set R7 as an input
	OUTPORT_TRIS = OUTPORT_TRIS | 0b00001000;
	// Set Read
	RW_PIN = 1;
	RS_PIN = 0;
	usDELAY(50);
	while(!breakchar)
	{
		toggleClock();
		Nop(); //Delay 1us
		if(BUSY_IN == 0)
			breakchar = 1;
		toggleClock();
		Nop();
		
	}
	OUTPORT_TRIS = OUTPORT_TRIS & 0b11110111;
}


void lcd_write(char *stringIn)
{

	while(*stringIn != '\0')
	{
		lcd_sendData(*stringIn);
		stringIn++;
	}

}


void lcd_init()
{
	OUTPORT_TRIS = 0x00;
	RS_TRIS = 0;
	RW_TRIS = 0;
	E_TRIS = 0;
	E_PIN = 0;
	RW_PIN = 0;
	RS_PIN = 0;

	OUTPORT = 0;
	DELAY(10);
	OUTPORT = 0x03;
	DELAY(4);
	toggleClock();
	DELAY(1);
	toggleClock();
	DELAY(1);
	toggleClock();
	DELAY(1);
	OUTPORT = 0x02;
	toggleClock();

	// 4 Bit, 5x8, 2 line display. Won't change.
	command(0x28);
	lcd_waitUntilReady();

	// Setup Display
	lcd_setEntry(ID_RIGHT & SHIFT_OFF);
	lcd_setDisplay(DISPLAY_ON & CURSOR_OFF & BLINK_OFF);	
	lcd_setLine(1);

}
