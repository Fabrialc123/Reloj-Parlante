
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <system.h>
#include <uart.h>
// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI
static void port_init( void );
static void install_dummy_isr( void );
static void show_sys_info( void );

void isr_SWI_dummy( void)        __attribute__ ((interrupt ("SWI")));
void isr_UNDEF_dummy( void )     __attribute__ ((interrupt ("UNDEF")));
void isr_IRQ_dummy( void )       __attribute__ ((interrupt ("IRQ")));
void isr_FIQ_dummy( void )       __attribute__ ((interrupt ("FIQ")));
void isr_PABORT_dummy( void )    __attribute__ ((interrupt ("ABORT")));
void isr_DABORT_dummy( void )    __attribute__ ((interrupt ("ABORT")));
void isr_ADC_dummy( void )       __attribute__ ((interrupt ("IRQ")));
void isr_RTC_dummy( void )       __attribute__ ((interrupt ("IRQ")));
void isr_TICK_dummy( void )		__attribute__ ((interrupt ("IRQ")));
void isr_USB_dummy( void )       __attribute__ ((interrupt ("IRQ")));

void sys_init( void )
{
    WTCON  = 0;    
    INTMSK = ~0;

    LOCKTIME = 0xFFF;
    PLLCON   = 0x38021;
    CLKSLOW  = 0x8;
    CLKCON   = 0x7FF8;
    
    SBUSCON = 0x8000001B;
    
    SYSCFG = 0x0;
    
    I_PMST = 0x1F1B;
    I_PSLV = 0x1B1B1B1B;
    INTMOD = 0x0;
    install_dummy_isr();
    EXTINTPND = ~0x0;
    I_ISPC = 0xff;
    INTCON = 0x01;

    SET_OPMODE( SVCMODE );
    SET_IRQFLAG( 0 );
    SET_FIQFLAG( 1 );

    port_init();
    uart0_init();

    show_sys_info();
}

static void port_init( void )
{
    PDATA = ~0;
    PCONA = 0xFE;

    PDATB = ~0;
    PCONB = 0x14F;

    PDATC = ~0;
    PCONC = 0x5FF555FF;
    PUPC  = 0x94FB;

    PDATD = ~0;
    PCOND = 0xAAAA;
    PUPD  = 0xFF;
    
    PDATE = ~0;
    PCONE = 0x255A9;
    PUPE  = 0x1FB;

    PDATF = ~0;
    PCONF = 0x251A;
    PUPF  = 0x74;
    
    PDATG = ~0;
    PCONG = 0xF5FF;
    PUPG  = 0x30;

    SPUCR = 0x7;
    
    EXTINT = 0x22000220;
}

static void install_dummy_isr( void )
{ 
    pISR_TICK = (uint32)isr_TICK_dummy;
}        

void isr_SWI_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_SWI_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_UNDEF_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_UNDEF_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_IRQ_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_IRQ_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_FIQ_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_FIQ_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_PABORT_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_PABORT_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_DABORT_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_DABORT_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_ADC_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_ADC_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_RTC_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_RTC_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_UTXD1_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_UTXD1_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_UTXD0_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_UTXD0_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_SIO_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_SIO_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_IIC_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_IIC_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_URXD1_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_URXD1_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_URXD0_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_URXD0_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER5_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER5_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER4_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER4_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER3_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER3_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER2_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER2_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER1_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER1_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TIMER0_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TIMER0_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_UERR01_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_UERR01_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_WDT_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_WDT_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_BDMA1_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_BDMA1_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_BDMA0_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_BDMA0_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_ZDMA1_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_ZDMA1_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_ZDMA0_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_ZDMA0_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TICK_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TICK_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_PB_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_PB_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_ETHERNET_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_ETHERNET_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_TS_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TS_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_KEYPAD_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_KEYPAD_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_USB_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_USB_dummy" );
    SEGS = 0x75;
    while( 1 );
}

static void show_sys_info( void )
{
}

inline void sleep( void )
{
    CLKCON |= (1 << 2);    // Pone a la CPU en estado IDLE
}

