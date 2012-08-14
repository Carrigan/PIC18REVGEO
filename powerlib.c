#include "powerlib.h"
#include <p18f26k22.h>

#define PWR_PIN LATCbits.LATC4
#define PWR_TRIS TRISCbits.TRISC4

void pwr_on(void)
{
    // Output
    PWR_TRIS = 0;
    PWR_PIN = 1;

}

void pwr_off(void)
{
    PWR_PIN = 0;
}
