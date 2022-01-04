
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI
extern void isr_TIMER0_dummy( void );

static uint32 loop_ms = 0;
static uint32 loop_s = 0;

static void sw_delay_init( void );

void timers_init( void )
{
    TCFG0 = 0x0;
    TCFG1 = 0x0;

    TCNTB0 = 0x0;
    TCMPB0 = 0x0;
    TCNTB1 = 0x0;
    TCMPB1 = 0x0;
    TCNTB2 = 0x0;
    TCMPB2 = 0x0;
    TCNTB3 = 0x0;
    TCMPB3 = 0x0;
    TCNTB4 = 0x0;
    TCMPB4 = 0x0;
    TCNTB5 = 0x0;

    TCON = 0x0 | (1 << 1) | (1 << 9) | (1 << 13) | (1 << 17) | (1 << 21) | (1 << 25);
    TCON = 0x0;

    sw_delay_init();
}

static void sw_delay_init( void )
{
    uint32 i;
    
    timer3_start();
    for( i=1000000; i; i--);
    loop_s = ((uint64)1000000*10000)/timer3_stop();
    loop_ms = loop_s / 1000;
};

void timer3_delay_ms( uint16 n )
{
    for(; n; n-- )
    {
    	TCFG0 = (TCFG0 & ~(0xff << 8)) | (0 << 8);
    	TCFG1 = (TCFG1 & ~(0xf << 12)) | (0 << 12);
    	TCNTB3 =  32000;
    	TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    	TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    	while( !TCNTO3 );
    	while( TCNTO3 );
    }
}

void sw_delay_ms( uint16 n )
{
    uint32 i;
    
    for( i=loop_ms*n; i; i-- );
}

void timer3_delay_s( uint16 n )
{
    for(; n; n-- )
    {
    	TCFG0 = (TCFG0 & ~(0xff << 8)) | (63 << 8);
    	TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    	TCNTB3 =  31250;					//(uint16) (mclk/2048)
    	TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    	TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    	while( !TCNTO3 );
    	while( TCNTO3 );
    }
}

void sw_delay_s( uint16 n )
{
    uint32 i;
    
    for( i=loop_s*n; i; i-- );
}

void timer3_start( void ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);    
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = 0xffff; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_stop( void )
{
    TCON &= ~(1 << 16);
    return 0xffff - TCNTO3;
}

void timer3_start_timeout( uint16 n ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);          
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = n; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_timeout( )
{
    return !TCNTO3;
}    

void timer0_open_tick( void (*isr)(void), uint16 tps )
{
    pISR_TIMER0 = (uint32)isr;
    I_ISPC      = BIT_TIMER0;
    INTMSK     &= ~(BIT_GLOBAL | BIT_TIMER0 );

    if( tps > 0 && tps <= 10 ) {		// Resolucion a 25 us  ((N + 1) * D) / 64 MHz) = 2.5 us -> ((N + 1) * D) = 1600 -> N = 100, D = 16
        TCFG0  = (TCFG0 & ~(0xff << 0)) | (100 << 0);
        TCFG1  = (TCFG1 & ~(0xf << 0)) | (3 << 0);
        TCNTB0 = (40000U / tps);
    } else if( tps > 10 && tps <= 100 ) {		// Resolucion a 2.5 us	((N + 1) * D) / 64 MHz) = 2.5 us -> ((N + 1) * D) = 160 -> N = 10, D = 16
        TCFG0  = (TCFG0 & ~(0xff << 0)) | (10 << 0);
         TCFG1  = (TCFG1 & ~(0xf << 0)) | (3 << 0);
        TCNTB0 = (400000U / (uint32) tps);
    } else if( tps > 100 && tps <= 1000 ) {		// Resolucion a 0.25 us		((N + 1) * D) / 64 MHz) = 0.25 us -> ((N + 1) * D) = 16 -> N = 0, D = 16
        TCFG0  = (TCFG0 & ~(0xff << 0)) | (0 << 0);
        TCFG1  = (TCFG1 & ~(0xf << 0)) | (3 << 0);
        TCNTB0 = (4000000U / (uint32) tps);
    } else if ( tps > 1000 ) {				// Resolucion a 31.25 ns
        TCFG0  = (TCFG0 & ~(0xff << 0)) | (0 << 0);
        TCFG1  = (TCFG1 & ~(0xf << 0)) | (0 << 0);
        TCNTB0 = (32000000U / (uint32) tps);
    }

    TCON = (TCON & ~(0xf << 0)) | (1 << 3) | (1 << 1) ;
    TCON = (TCON & ~(0xf << 0)) | (1 << 3) | (1 << 0) ;
}

void timer0_open_ms( void (*isr)(void), uint16 ms, uint8 mode )
{
    pISR_TIMER0 = (uint32)isr;
    I_ISPC      = BIT_TIMER0;
    INTMSK     &= ~(BIT_GLOBAL | BIT_TIMER0 );
    // Resolucion a 100 us   ((N + 1) * D) / 64 MHz) = 100 us -> ((N + 1) * D) = 6400 -> N = 199, D = 32
    TCFG0  = (TCFG0 & ~(0xff << 0)) | (100 << 0);
    TCFG1  = (TCFG1 & ~(0xf << 0)) | (4 << 0);
    TCNTB0 = 10*ms;

    TCON = (TCON & ~(0xf << 0)) | (mode << 3) | (1 << 1) ;
    TCON = (TCON & ~(0xf << 0)) | (mode << 3) | (1 << 0) ;
}

void timer0_close( void )
{
    TCNTB0 = 0x0;
    TCMPB0 = 0x0;

    TCON = (TCON & ~(0xf << 0)) | (1 << 1) ;
    TCON = (TCON & ~(0xf << 0)) ;
    
    INTMSK     |= BIT_TIMER0;
    pISR_TIMER0 = (uint32)isr_TIMER0_dummy;
}

