
#include <s3c44b0x.h>
#include <uart.h>
// Modificado por Fabrizio Alcaraz Escobar 3ro D PSyD UCM FDI
void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0x0;
    ULCON0 = 0x3;
    UBRDIV0 = 0x22;
    UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
    while( UFSTAT0 & (1 << 9) );
    UTXH0 = ch;
}        

char uart0_getchar( void )
{
    while( (~UFSTAT0 & 0x1) && (~UFSTAT0 & (1 << 1)) && (~UFSTAT0 & (1 << 2)) && (~UFSTAT0 & (1 << 3)) );
    return URXH0;
}

void uart0_puts( char *s )
{
    while (*s != '\0'){
    	uart0_putchar(*s);
    	s++;
    }
}

void uart0_putint( int32 i )
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

    uart0_puts( p );
}

void uart0_puthex( uint32 i )
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

    uart0_puts( p );
}

void uart0_gets( char *s )
{
    *s = uart0_getchar();
    while (*s != '\n'){
    	s++;
    	*s = uart0_getchar();
    }
    *s = '\0';
}

int32 uart0_getint( void )
{
    char buf[256];		// Reservamos de mas por si el usuario escribe un numero muy grande
    char *p = buf;
    char esNegativo;
    int32 res = 0;
    uint8 c = 0;

    uart0_gets(p);
    if (*p == '-') {
    	esNegativo = *p++;
    }
    while (*p != '\0'){
    	c = *p - '0';
    	res = (res * 10) + c;
    	p++;
    }
    if (esNegativo == '-')  res = ~res + 1  ;
	return res;
}

uint32 uart0_gethex( void )
{
    char buf[8 + 1];
    char *p = buf;
    uint32 res = 0;
    uint8 c = 0;

    uart0_gets(p);

    while (*p != '\0'){
    	if ((*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p<='F' ) ) {
    		if (*p <= 'F') *p += 32;
    		c = *p - 'a' + 10;
    	}
    	else c = *p - '0';
    	res = (res * 16) + c;
    	p++;
    }
	return res;
}
