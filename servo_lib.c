#include <timers.h>
#include <p18f26k22.h>
#include "servo_lib.h"

#define SERVO_PIN LATCbits.LATC0
#define SERVO_PIN_TRIS TRISCbits.TRISC0

#define SERVO_LENGTH 20000 		//  20 ms
#define SERVO_MIN 500			// .5  ms
#define SERVO_MAX 2500			// 2.5 ms
#define SERVO_SCALE 11			// Add 500 then multiply by this for an angle (0-180)
#define SERVO_ANGLE_DEFAULT 90

int SERVO_ANGLE;

void servo_write(int ANGLE)
{
	SERVO_ANGLE = 65535-(500+(SERVO_SCALE*ANGLE));
}

void servo_init(void)
{
	// Set servo as output
	SERVO_PIN_TRIS = 0;
	
	SERVO_ANGLE = 65535-(500+(SERVO_SCALE*SERVO_ANGLE_DEFAULT));

	// Open timers 
	INTCON = 0x20;                //disable global and enable TMR0 interrupt
  	INTCON2 = 0x84;               //TMR0 high priority
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1;
  	RCONbits.IPEN = 1;            //enable priority levels
  	T0CON = 0b10000011;           //set up timer0 - prescaler 1:16
	T1CON = 0b00000010;
	
	// Set 20ms pulse rate	
  	TMR0H = 0xFB;
  	TMR0L = 0x1D;
}

void servo_timerHandlerFast(void)
{
		// Set servo pin
		SERVO_PIN = 0;
		
		// Turn off timer 1
		T1CONbits.TMR1ON = 0;

		// Reset timer 1
		PIR1bits.TMR1IF = 0;
}

void servo_timerHandlerSlow(void)
{
		unsigned int TMRNO;
		// Set to 20ms
		TMR0H = 0xFB;
  		TMR0L = 0x1D;

		SERVO_PIN = 1;	

		// Calculate the servo timing pulse
		TMRNO = SERVO_ANGLE;

		// Set TMR1 time
		TMR1H = TMRNO / 256;
		TMR1L = TMRNO % 256;
		T1CONbits.TMR1ON = 1;

		// Reset interrupt
		INTCONbits.TMR0IF = 0;
}
