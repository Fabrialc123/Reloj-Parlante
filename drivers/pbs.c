
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>
// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI
extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( (PDATG & PB_LEFT) == 0 )
        return PB_LEFT;
    else if((PDATG & PB_RIGHT) == 0)
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{
	if ((PDATG & scancode) == 0) return 0x1;
	else return 0x0;
}

void pb_wait_keydown( uint8 scancode )
{
    while( pb_status(scancode) != PB_DOWN );
    sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_keyup( uint8 scancode )
{
    while( pb_status(scancode) != PB_DOWN );
    sw_delay_ms( PB_KEYDOWN_DELAY );
    while( pb_status(scancode) != PB_UP );
    sw_delay_ms( PB_KEYUP_DELAY );
}

void pb_wait_any_keydown( void )
{
    while(pb_scan() == PB_FAILURE );
    sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_any_keyup( void )
{
	while(pb_scan() == PB_FAILURE );
	sw_delay_ms( PB_KEYDOWN_DELAY );
	while( pb_scan() != PB_FAILURE );
	sw_delay_ms( PB_KEYUP_DELAY );
}

uint8 pb_getchar( void )
{
    uint8 scancode;

    while( pb_scan() == PB_FAILURE );
    sw_delay_ms( PB_KEYDOWN_DELAY );

    scancode = pb_scan();

    while( pb_scan() != PB_FAILURE );
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( pb_scan() == PB_FAILURE );
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( pb_scan() != PB_FAILURE );
    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
    uint8 scancode;

    timer3_start_timeout(10 * ms);
    while( pb_scan() == PB_FAILURE || !timer3_timeout() );
    sw_delay_ms( PB_KEYDOWN_DELAY );

    scancode = pb_scan();

    while(pb_scan() != PB_FAILURE || !timer3_timeout());
    if (timer3_timeout()) scancode = PB_TIMEOUT;
    timer3_stop();
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32)isr;
    EXTINTPND |= (BIT_RIGHTPB| BIT_LEFTPB);
    I_ISPC    = BIT_PB;
    INTMSK   &= ~(BIT_GLOBAL | BIT_PB );
}

void pbs_close( void )
{
    INTMSK  |= BIT_PB;
    pISR_PB  = (uint32)isr_PB_dummy;
}
