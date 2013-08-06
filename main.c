#include <p18f26k22.h>
#include <delays.h>
#include <usart.h>
#include <stdio.h>
#include <stdlib.h>
#include "servo_lib.h"
#include "LCD_lib.h"
#include "powerlib.h"

// Added 8/5/2013
#include "gpggalib.h"
#include "gpsmath.h"

#pragma config FOSC = INTIO67
#pragma config HFOFST = ON
#pragma config WDTEN = OFF


// Writes 0 to found and allows you to lock the box
//#define RESET_PROG

void InterruptHandlerHigh(void);
void ifFix(void);

char acquiringString[] = "Acquiring GPS...";
char acquiredString[] = "GPS Acquired!   ";
char distanceString[] = "Distance:       ";
char shutdownString[] = "Shutting down:  ";
char timeoutString[] =  "Timeout in      ";
char milesString[] = 	"           miles";

char gotItString[] =	"Congrats big bro";
char gotIt2String[] = 	"Opening the box.";
char blankString[] = 	"                ";
char distanceCalc[17];

static volatile char gpsDataCount;
static unsigned char a;
static unsigned char b;

void main()
{
	// Set the OR gate pin high so that we stay on...
	pwr_on();
	
	// Run the clock at 4MHZ, .25 INST/Cycle, 1uS/inst
	OSCCONbits.IRCF = 0b101;

	// Set up UART
	ANSELC = 0;
	Open1USART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
				USART_CONT_RX & USART_BRGH_HIGH, 51);

	BAUDCON1bits.DTRXP = 0;
	BAUDCON1bits.CKTXP = 0;

	// Initialize Peripherals
	gpgga_init();
	lcd_init();
	servo_init();
	gpsDataCount = 0;

	// Enable Global Interrupts
	INTCONbits.GIEH = 1;

	// Start up	
	lcd_write(acquiringString);	 
	
	while(1)
	{
		static int timeout = 180;

		// Wait for 10 valid strings
		if(gpsDataCount > 10)
		{
			lcd_setLine(1);
			lcd_write(acquiredString);
			lcd_setLine(2);
			lcd_write(blankString);
			ifFix();			
		}
	
		if(timeout == 0)
		{
			pwr_off();
			Delay10KTCYx(200);
			Delay10KTCYx(200);
			Delay10KTCYx(200);
			servo_write(120);
		}

		lcd_setLine(2);
		if(timeout > 99)
		{
			itoa(timeout, &timeoutString[11]);
		} else if (timeout > 9) {
			itoa(timeout, &timeoutString[12]);
			timeoutString[11] = ' ';
		} else {
			itoa(timeout, &timeoutString[13]);
			timeoutString[11] = ' ';	
			timeoutString[12] = ' ';			
		}	 
		lcd_write(timeoutString);

		// Delay ~1s
		Delay10KTCYx(80);

		timeout--;
	}
}

void ifFix(void)
{
	// Process the distance into a usable form
	float currentDistance = gpsmath_computeDistance(gpgga_getLastGpsData(), 41.0, 67.0);
	int pre_dec = (int)(currentDistance);
	long post_dec = (long)((currentDistance * 10000) - (long)pre_dec*10000);
	char *temp_ptr;
	char countdown = 9;
	
	lcd_setLine(2);
	lcd_write(distanceString);
	
	// Print the whole number part of the distance
	itoa(pre_dec, distanceCalc);
	
	// Find the 0
	temp_ptr = distanceCalc;
	while(*temp_ptr != 0)	temp_ptr++;
	
	// Replace 0 with a decimal then increment
	*temp_ptr = '.';	
	temp_ptr++;		
	
	// Write the remainder
	ltoa(post_dec, temp_ptr);
	
	// Write this to LCD
	lcd_setLine(3);
	lcd_write(milesString);
	lcd_setLine(3);
	lcd_write(distanceCalc);
	
	// If we are within .25 miles:
	if(currentDistance < .25)
	{
	
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		
		servo_write(120);
		lcd_setLine(1);
		lcd_write(blankString);
		lcd_setLine(2);
		lcd_write(gotItString);
		lcd_setLine(3);
		lcd_write(gotIt2String);
		lcd_setLine(4);
		lcd_write(blankString);
	
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		Delay10KTCYx(100);
		pwr_off();
	} else {
		// Shutdown sequence
		while(countdown != 0)
		{
			itoa(countdown, &shutdownString[15]);
			lcd_setLine(4);
			lcd_write(shutdownString);
			countdown--;
			Delay10KTCYx(100);
		}
		
		pwr_off();
		Delay10KTCYx(200);
		Delay10KTCYx(200);
		Delay10KTCYx(200);
		servo_write(120);
  	}	 		
}

#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
	_asm
	goto InterruptHandlerHigh
	_endasm
}
#pragma code

#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh()
{
	// If interrupt is from GPS RX:
	if(PIR1bits.RC1IF)
	{
		unsigned char temp = Read1USART();
		gpsDataCount += gpgga_feedCharacter(temp);
	}
	// If interrupt is from timer 0 (slow):
	if(INTCONbits.TMR0IF)
	{
		servo_timerHandlerSlow();
	}
	// If interrupt is from timer 1 (fast):
	if(PIR1bits.TMR1IF)
	{		
		servo_timerHandlerFast();	
	}
}
