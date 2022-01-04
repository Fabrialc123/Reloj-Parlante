#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <adc.h>
#include <lcd.h>
#include <ts.h>

// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI

#define PX_ERROR    (5)

static uint16 Vxmin = 0;
static uint16 Vxmax = 0;
static uint16 Vymin = 0;
static uint16 Vymax = 0;

static uint8 state;

extern void isr_TS_dummy( void );

static void ts_scan( uint16 *Vx, uint16 *Vy );
static void ts_calibrate( void );
static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y );

void ts_init( void )
{
    lcd_init();
    adc_init();
    PDATE = (PDATE & ~(0xf << 4)) | (1 << 7) | (0 << 6) | (1 << 5) | (1 << 4);
    ts_on();
    ts_calibrate();
    ts_off();
}

void ts_on( void )
{
    adc_on();
    state = TS_ON;
}

void ts_off( void )
{
    adc_off();
    state = TS_OFF;
}

uint8 ts_status( void )
{
    return state;
}

static void ts_calibrate( void )
{
    uint16 x, y;
    uint16 Vx, Vy;
    
    lcd_on();
    lcd_clear();
    do {

    	lcd_draw_hline(0,PX_ERROR,0, BLACK , PX_ERROR);
    	lcd_puts(10,200,BLACK, "Calibrando la pantalla...");
    	lcd_puts(10,220,BLACK, "Presione la esquina superior izquierda");

        while( (PDATG & (1 << 2)) != 0 );
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmin, &Vymax );
        while( (PDATG & (1 << 2)) != 0x4 );
        sw_delay_ms( TS_UP_DELAY );

        lcd_draw_hline(LCD_WIDTH - 1 - PX_ERROR,LCD_WIDTH - 1, LCD_HEIGHT - 1 - PX_ERROR, BLACK , PX_ERROR);
    	lcd_draw_hline(0,LCD_WIDTH - 1, 220, WHITE , 16);
    	lcd_puts(10,220,BLACK, "Presione la esquina inferior derecha");
           
        while( (PDATG & (1 << 2)) != 0 );
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmax, &Vymin );
        while( (PDATG & (1 << 2)) != 0x4 );
        sw_delay_ms( TS_UP_DELAY );
    
        lcd_draw_hline((LCD_WIDTH - PX_ERROR)/2,(LCD_WIDTH + PX_ERROR)/2, (LCD_HEIGHT - PX_ERROR)/2, BLACK , PX_ERROR);
    	lcd_draw_hline(0,LCD_WIDTH - 1, 220, WHITE , 16);
    	lcd_puts(10,220,BLACK, "Presione el centro");
    
        while( (PDATG & (1 << 2)) != 0 );
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vx, &Vy );
        while( (PDATG & (1 << 2)) != 0x4 );
        sw_delay_ms( TS_UP_DELAY );
        ts_sample2coord( Vx, Vy, &x, &y );

        lcd_clear();
    } while( (x > LCD_WIDTH/2+PX_ERROR) || (x < LCD_WIDTH/2-PX_ERROR) || (y > LCD_HEIGHT/2+PX_ERROR) || (y < LCD_HEIGHT/2-PX_ERROR) );
    
}

void ts_wait_down( void )
{
    while( (PDATG & (1 << 2)) == 0 );
    sw_delay_ms( TS_DOWN_DELAY );
}

void ts_wait_up( void )
{
    while( (PDATG & (1 << 2)) == 0 );
    sw_delay_ms( TS_DOWN_DELAY );
    while( (PDATG & (1 << 2)) == 0x4 );
    sw_delay_ms( TS_UP_DELAY );
}

void ts_getpos( uint16 *x, uint16 *y )
{
    uint16 Vx, Vy;

    while( (PDATG & (1 << 2)) == 0 );
    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );
    ts_sample2coord( Vx, Vy, x, y );

    while( (PDATG & (1 << 2)) == 0x4 );
    sw_delay_ms( TS_UP_DELAY );
}

void ts_getpostime( uint16 *x, uint16 *y, uint16 *ms )
{
	uint16 Vx, Vy;

    while( (PDATG & (1 << 2)) != 0 );

    timer3_start();

    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );
    ts_sample2coord( Vx, Vy, x, y );
    while( (PDATG & (1 << 2)) != 0x4 );

    *ms = timer3_stop() / 10;

    sw_delay_ms( TS_UP_DELAY );
}

uint8 ts_timeout_getpos( uint16 *x, uint16 *y, uint16 ms )
{
	uint16 Vx, Vy;

	timer3_start_timeout(10 * ms);

    while( (PDATG & (1 << 2)) == 0 );

    timer3_start();

    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );
    ts_sample2coord( Vx, Vy, x, y );
    while( (PDATG & (1 << 2)) == 0x4 );

    if (timer3_timeout()) return TS_TIMEOUT;
    timer3_stop();

    sw_delay_ms( TS_UP_DELAY );

    return TS_OK;
}

static void ts_scan( uint16 *Vx, uint16 *Vy )
{
    PDATE =(0 << 7) | (1 << 6) | (1 << 5) | (0 << 4);
    *Vx = adc_getSample( ADC_AIN1 );
    
    PDATE =(1 << 7) | (0 << 6) | (0 << 5) | (1 << 4);
    *Vy = adc_getSample( ADC_AIN0 );
    
    PDATE =(1 << 7) | (0 << 6) | (1 << 5) | (1 << 4);
}

static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y )
{
    if( Vx < Vxmin )
        *x = 0;
    else if( Vx > Vxmax )
        *x = LCD_WIDTH-1;
    else 
        *x = LCD_WIDTH*(Vx-Vxmin) / (Vxmax-Vxmin);

    if (Vy < Vymin){
    	*y = LCD_HEIGHT - 1;
    }
    else if (Vy > Vymax){
    	*y = 0;
    }
    else *y = LCD_HEIGHT * ((Vy - Vymax) * -1)/ (Vymax - Vymin);
}

void ts_open( void (*isr)(void) )
{
	pISR_TS = (uint32) isr;
    I_ISPC   = (BIT_TS);
    INTMSK  &= ~(BIT_GLOBAL | BIT_TS );
}

void ts_close( void )
{
    INTMSK  |= (BIT_TS);
    pISR_TS = (uint32) isr_TS_dummy;
}

