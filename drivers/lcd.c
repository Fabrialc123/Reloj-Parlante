
#include <s3c44b0x.h>
#include <lcd.h>

// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;

void lcd_init( void )
{      
    DITHMODE = 0x12210;
    DP1_2    = 0xA5A5;
    DP4_7    = 0xBA5DA65;
    DP3_5    = 0xA5A5F;
    DP2_3    = 0xD6B;
    DP5_7    = 0xEB7B5ED;
    DP3_4    = 0x7DBE;
    DP4_5    = 0x7EBDF;
    DP6_7    = 0x7FDFBFE;
    
    REDLUT   = 0x0;
    GREENLUT = 0x0;
    BLUELUT  = 0x0;

    LCDCON1  = 0x1C020;
    LCDCON2  = 0x13CEF;
    LCDCON3  = 0x0;

    LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
    LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
    LCDSADDR3 = 0x50;
    
    lcd_off();
}

void lcd_on( void )
{
	LCDCON1 |= 1 << 0;
    state = ON;
}

void lcd_off( void )
{
	LCDCON1 &= ~(1 << 0);
    state = OFF;
}

uint8 lcd_status( void )
{
    return state;
}

void lcd_clear( void )
{
	uint16 i,j;
    for (i = 0; i < LCD_WIDTH ; i++){
    	for (j = 0; j < LCD_HEIGHT; j++){
    		lcd_putpixel(i, j, WHITE);
    	}
    }
}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    
    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
    uint8 byte, bit, pixel;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;

    byte = lcd_buffer[i];

    if (bit == 0) pixel = byte & ~0x0F;
    else pixel = (byte >> 4);

    return pixel;
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{
	uint16 i,j;
	for (i = 0; i < width;i++){
	   for (j = xleft; j < xright;j++){
		   lcd_putpixel(j, y+i,color);
	   }
   }
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{
	uint16 i,j;
	for (i = 0; i < width;i++){
	   for (j = yup; j < ydown;j++){
		   lcd_putpixel(x + i, j,color);
	   }
   }
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
    lcd_draw_hline(xleft,xright,yup,color,width);
    lcd_draw_vline(yup,ydown,xleft,color,width);
    lcd_draw_hline(xleft,xright,ydown - width,color,width);
    lcd_draw_vline(yup,ydown,xright - width,color,width);
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )                    
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	uint16 pos;
	pos = x;
    while(*s != '\0'){
    	lcd_putchar(pos,y,color,*s);
    	pos += 8;
    	s++;
    }
}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
    char buf[11 + 1];		/* 10 digitos posibles + el signo + '\0' */
    char *p = buf + 11;
    uint8 c;
    int32 copia;
    if (i < 0) copia = ~i + 1;
    else copia = i;

    *p = '\0';

    do {
    	c = copia % 10;
    	*--p = '0' + c;
    	copia = copia /10;
    } while( copia );

    if (i < 0) *--p = '-';

    lcd_puts(x,y,color, p );
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    lcd_puts(x,y,color,p);
}

void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<32; line++ )
        for( row=0; row<16; row++ ){
            if( bitmap[line/2] & (0x80 >> row/2) ){
                lcd_putpixel( x+row, y+line, color );
            }
            else{
                lcd_putpixel( x+row, y+line, WHITE );
            }
        }
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
	uint16 pos;
	pos = x;
    while(*s != '\0'){
    	lcd_putchar_x2(pos,y,color,*s);
    	pos += 16;
    	s++;
    }
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
    char buf[11 + 1];		/* 10 digitos posibles + el signo + '\0' */
    char *p = buf + 11;
    uint8 c;
    int32 copia;
    if (i < 0) copia = ~i + 1;
    else copia = i;

    *p = '\0';

    do {
    	c = copia % 10;
    	*--p = '0' + c;
    	copia = copia /10;
    } while( copia );

    if (i < 0) *--p = '-';

    lcd_puts_x2(x,y,color, p );
}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    lcd_puts_x2(x,y,color,p);
}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;
    
    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )                                                                       
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}
