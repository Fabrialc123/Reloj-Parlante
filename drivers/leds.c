
#include <s3c44b0x.h>
#include <leds.h>
// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI
void leds_init( void )
{
	PCONB &= ~( (1<<10) | (1<<9) );   // PB[10] = out, PB[9] = out
    led_off(RIGHT_LED);
    led_off(LEFT_LED);
}

void led_on( uint8 led )
{
    PDATB &= ~(led << 9);		// Pone a 0
}

void led_off( uint8 led )
{
    PDATB |= (led << 9);	// Pone a 1
}

void led_toggle( uint8 led )
{
	PDATB ^= (led << 9);
}

uint8 led_status( uint8 led )
{
    if (PDATB & (led << 9)) return 0x00;		// Si es 1 está apagado
    else return 0x1;
}
