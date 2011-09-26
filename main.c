#include <p18f26k22.h>
#include <delays.h>

#pragma config FOSC = INTIO67
#pragma config HFOFST = ON


#define LED LATCbits.LATC0
#define LEDtris TRISCbits.TRISC0

void main()
{
	LEDtris = 0;
	LED = 1;

	while(1)
	{
		LED = ~LED;
		Delay10KTCYx(25);
	}
	
}