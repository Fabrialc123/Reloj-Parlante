
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <system.h>
#include <uart.h>
#include <uda1341ts.h>
#include <lcd.h>
#include <iis.h>
#include "sintesisVoz.h"
#include <rtc.h>
#include <ts.h>
#include <timers.h>
#include <pbs.h>
// Hecho por Fabrizio Alcaraz Escobar 3ro D PSyD FDI UCM
//ICONOS
#define VOLUMEN_ICO 		(0)
#define TEMPORIZADOR_ICO 	(1)
#define CRONOMETRO_ICO 		(2)
#define HORA_ICO 			(3)
#define FECHA_ICO 			(4)
#define ALARMA_ICO 			(5)
#define VOZ_ICO 			(6)

//MODOS
#define VOLUMEN_MODO 		(0)
#define TEMPORIZADOR_MODO 	(1)
#define CRONOMETRO_MODO 	(2)
#define HORA_MODO 			(3)
#define FECHA_MODO			(4)
#define ALARMA_MODO			(5)
#define VOZ_MODO			(6)

#define MODOS (7)

//DESCRIPCIONES
const uint8 descripciones[MODOS * 5] = {
'V','O','L',' ','\0',
'T','E','M','P','\0',
'C','R','O','N','\0',
'H','O','R','A','\0',
'F','E','C','H','\0',
'A','L','R','M','\0',
'V','O','Z',' ','\0'
};
//Posiciones
const uint16 posiciones [MODOS * 2]= {
179, 4, 	// VOLUMEN
7, 60,		//TEMPORIZADOR
7, 119,		//CRONOMETRO
276, 60,	//HORA
276, 119,	//FECHA
101, 4,		//ALARMA
139, 178		//VOZ
};

extern uint16 icons[];
extern uint8 font[];

void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));
void isr_ts( void ) __attribute__ ((interrupt ("IRQ")));
void isr_pbs( void ) __attribute__ ((interrupt ("IRQ")));


void dibujaIcono(uint16 x, uint16 y, uint8 color, uint8 icon, char *desc);
void apagaIcono(uint16 x, uint16 y, uint8 icon, char *desc);
void puticon(uint16 x, uint16 y, uint8 color, char ch);
void comprobarTS(void);
void hablar(char *txt);
void dibujarInterfaz(void);
void comprobarAlarma(void);
void dibujar_x3(uint16 x, uint16 y, uint8 color, char *s);
void dibujar_x3_Numeros(uint16 x, uint16 y, uint8 color, uint8 i);
void hablarNumeros(uint8 i);


void volumen_modo(void);
void temporizador_modo(void);
void cronometro_modo(void);
void hora_modo(void);
void fecha_modo(void);
void alarma_modo(void);
void voz_modo(void);


uint8 modo, modoAnterior;
volatile boolean alarmaActiva = FALSE;
rtc_time_t rtc_time, rtc_alarma;
volatile boolean flagTS = FALSE;
volatile boolean flagPBS = FALSE;
volatile boolean flagAlarma = FALSE;
volatile boolean flag1Seg = FALSE;

void main( void )
{
	sys_init();										 /* Inicializa sistemas y periféricos */
	uart0_init();
	lcd_init();
    uda1341ts_init();
    timers_init();
    rtc_init();
    ts_init();
    pbs_init();
    iis_init( IIS_POLLING );

    modo =  HORA_MODO;
    modoAnterior = modo;


    lcd_on();
    dibujarInterfaz();


    ts_on();
    ts_open(isr_ts);
    pbs_open(isr_pbs);
    rtc_open( isr_tick, 127 );

    rtc_gettime(&rtc_time);
    rtc_alarma = rtc_time;
    alarmaActiva = FALSE;

    uda1341ts_setvol( VOL_MED );
    hablar("Bienvenido. ");

    while( 1 ){
    	if  (modo != modoAnterior){
    		uart0_puts( "Se ha cambiado de modo  " );
    		uart0_putint(modoAnterior);
    		uart0_puts( " a modo  " );
    		uart0_putint(modo);
    		uart0_puts( "\n" );
    		uart0_puts( "ModoAnterior ahora es   " );
    		uart0_putint(modoAnterior);
    		uart0_puts( "\n" );
    		dibujarInterfaz();
    	}
    	flagPBS = FALSE;
    	flagTS = FALSE;
    	flag1Seg = FALSE;

    	switch (modo){
    	case VOLUMEN_MODO:
    		volumen_modo();
    		break;
    	case TEMPORIZADOR_MODO:
    		temporizador_modo();
    		break;
    	case CRONOMETRO_MODO:
    		cronometro_modo();
    		break;
    	case HORA_MODO:
    		hora_modo();
    		break;
    	case FECHA_MODO:
    		fecha_modo();
    		break;
    	case ALARMA_MODO:
    		alarma_modo();
    		break;
    	case VOZ_MODO:
    		//voz_modo();
    		modo = modoAnterior;
    		break;
    	default:
    		modo = HORA_MODO;
    		break;
    	}
    }

}

void isr_tick( void ){
  rtc_gettime( &rtc_time );
  flag1Seg = TRUE;
  if (alarmaActiva == TRUE && rtc_time.hour == rtc_alarma.hour && rtc_time.min == rtc_alarma.min) flagAlarma = TRUE;
  I_ISPC = BIT_TICK;
}
void isr_ts(void){
	flagTS = TRUE;
	I_ISPC = BIT_TS;
}
void isr_pbs(void){
    flagPBS = TRUE;
    EXTINTPND = BIT_RIGHTPB;
    EXTINTPND = BIT_LEFTPB;
    I_ISPC = BIT_PB;
}
void puticon( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint16 *bitmap;

    bitmap = icons + ch*16;
    for( line=0; line<32; line++ )
        for( row=0; row<32; row++ )
            if( bitmap[line/2] & (0x8000 >> row/2) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void dibujaIcono(uint16 x, uint16 y, uint8 color, uint8 icon, char *desc){
    lcd_puts(x + 2,y + 32,BLACK,desc);
    if (icon == ALARMA_ICO && alarmaActiva == FALSE)puticon(x + 2, y + 2 ,0x2,icon);
    else puticon(x + 2, y + 2 ,BLACK,icon);
    lcd_draw_box(x,y,x+35,y+47, color, 2 );
}

void apagaIcono(uint16 x, uint16 y, uint8 icon, char *desc){
	lcd_puts(x + 2,y + 32,BLACK,desc);
	if (icon == ALARMA_ICO && alarmaActiva == TRUE)puticon(x + 2, y + 2 ,BLACK,icon);
	else puticon(x + 2, y + 2 ,0x2,icon);
	lcd_draw_box(x,y,x+35,y+47, WHITE, 2 );
}

void comprobarTS(void){
	if (flagTS == TRUE){
	uint16 x, y, i;
	boolean encontrado = FALSE;

		ts_getpos( &x, &y );
        uart0_puts( " TS (" );
        uart0_putint( x );
        uart0_puts( ", " );
        uart0_putint( y );
        uart0_puts( ")\n" );

        for (i = 0; i < MODOS && !encontrado;i++){
        	if (x >= posiciones[i* 2] && x <= posiciones[i*2] + 35 && y >= posiciones[i*2 + 1] && y <= posiciones[i*2 + 1] + 47){
        		modo = i;
        		encontrado = TRUE;
        	}
        }
     flagTS = FALSE;
	}
}

void comprobarAlarma(void){
	uint8 volumen,i;
	if (flagAlarma == TRUE){
		volumen = uda1341ts_getvol();
		uda1341ts_setvol( VOL_MAX );
		dibujarInterfaz();
		dibujar_x3(86,81,BLACK,"ALARMA");
		for (i = 0; i < MODOS; i++){
			apagaIcono(posiciones[i * 2],posiciones[i * 2 + 1],i,&descripciones[i*5]);
		}
		while(!flagPBS){
			hablar("rr");
		}
		dibujarInterfaz();
		uda1341ts_setvol( volumen );
		alarmaActiva = FALSE;
		flagAlarma = FALSE;
		flagPBS = FALSE;
	}
}



void hablar(char *txt){
	dibujaIcono(posiciones[VOZ_MODO * 2],posiciones[VOZ_MODO * 2 + 1],BLACK,VOZ_ICO+1,&descripciones[VOZ_MODO*5]);
	iis_textToSpeech(txt);
	dibujaIcono(posiciones[VOZ_MODO * 2],posiciones[VOZ_MODO * 2 + 1],WHITE,VOZ_ICO,&descripciones[VOZ_MODO*5]);
}

void hablarNumeros(uint8 i){
    uint8 c;
    int32 copia;
    char buf[11 + 1];		/* 10 digitos posibles + el signo + '\0' */
    char *p = buf + 11;

    if (i < 0) copia = ~i + 1;
    else copia = i;

    *p = '\0';

    do {
    	c = copia % 10;
    	*--p = '0' + c;
    	copia = copia /10;
    } while( copia );

    if (i < 0) *--p = '-';

    hablar(p);
}

void dibujarInterfaz(void){
	uint8 i;

	lcd_clear();
	lcd_draw_hline(0,LCD_WIDTH - 1,0,BLACK,4);	// LINEA ARRIBA
	lcd_draw_hline(0,LCD_WIDTH - 1,226,BLACK,13);	//LINEA ABAJO
	lcd_draw_vline(0,LCD_HEIGHT - 1,0,BLACK,7);	// LINEA IZQUIERDA
	lcd_draw_vline(0,LCD_HEIGHT - 1,312,BLACK,8); // LINEA DERECHA

	lcd_draw_box(43,52,275,177,BLACK,2);

	for (i = 0; i < MODOS; i++){
		if (i == modo)  dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],BLACK,i,&descripciones[i*5]);
		else dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],WHITE,i,&descripciones[i*5]);
	}
}

void dibujar_x3(uint16 x, uint16 y, uint8 color, char *s){
	uint16 pos;
    uint8 line, row;
    uint8 *bitmap;


    while(*s != '\0'){
        bitmap = font + *s*16;
        for( line=0; line<48; line++ )
            for( row=0; row<24; row++ ){
                if( bitmap[line/3] & (0x80 >> row/3) ){
                    lcd_putpixel( x+row, y+line, color );
                }
                else{
                    lcd_putpixel( x+row, y+line, WHITE );
                }
            }
    	x += 24;
    	s++;
    }
}

void dibujar_x3_Numeros(uint16 x, uint16 y, uint8 color, uint8 i){
    uint8 c;
    int32 copia;
    char buf[11 + 1];		/* 10 digitos posibles + el signo + '\0' */
    char *p = buf + 11;

    copia = i;

    *p = '\0';

    do {
    	c = copia % 10;
    	*--p = '0' + c;
    	copia = copia /10;
    } while( copia );

    if (i <= 9) *--p = '0';


    dibujar_x3(x,y,color,p);

}

void volumen_modo(void){
	uint8 volumen;
	volumen = uda1341ts_getvol();
	dibujar_x3(86,81,BLACK,"Vol: ");
	dibujar_x3_Numeros(182,81,BLACK,volumen*100/VOL_MAX);
	hablar("Volumen");
	while (modo == VOLUMEN_MODO){
		if (flagPBS){
			if (pb_status(PB_RIGHT) == PB_DOWN){
				volumen += 9;
				if (volumen > VOL_MAX) volumen = VOL_MAX;
			}
			else if (pb_status(PB_LEFT) == PB_DOWN){
				volumen -= 9;
				if (volumen < 0 || volumen > VOL_MAX) volumen = 0;
			}
			dibujar_x3(86,81,BLACK,"Vol: ");
			dibujar_x3_Numeros(182,81,BLACK,volumen*100/VOL_MAX);
			if (volumen < VOL_MAX) dibujar_x3(230,81,BLACK," "); 		// Asi borramos el 0 que hubiera si antes habia un 100
			uda1341ts_setvol( volumen );
			hablar( "a." );
			flagPBS = FALSE;
		}
		dibujar_x3(86,81,BLACK,"Vol: ");
		dibujar_x3_Numeros(182,81,BLACK,volumen*100/VOL_MAX);
		if (volumen < VOL_MAX) dibujar_x3(230,81,BLACK," "); 		// Asi borramos el 0 que hubiera si antes habia un 100
		comprobarTS();
		if (modo == VOZ_MODO){
			hablar("Volumen al ");
			hablarNumeros(volumen*100/VOL_MAX);
			hablar("por ciento. ");
			modo = VOLUMEN_MODO;
		}
		comprobarAlarma();

	}



}
void temporizador_modo(void){
	uint8 mins, secs, i;
	mins = 5;
	secs = 0;
	boolean siguiente = FALSE;
	dibujar_x3_Numeros(97,83,0x3,mins);
	dibujar_x3(145,80,0x3,":");
	dibujar_x3_Numeros(169,83,0x3,secs);
	hablar("Temporizador");
	while (modo == TEMPORIZADOR_MODO){
		if (flagPBS == TRUE){
			if (pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN ){
				flagPBS= FALSE;
				dibujar_x3_Numeros(169,83,0x2,secs);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(97,83,BLACK,mins);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) mins = (mins + 1) % 100;
						else if (pb_status(PB_LEFT) == PB_DOWN) {
							if (mins == 0) mins = 99;
							else mins--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				dibujar_x3_Numeros(97,83,0x2,mins);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(169,83,BLACK,secs);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) secs = (secs + 1) % 60;
						else if (pb_status(PB_LEFT) == PB_DOWN){
							if (secs == 0) secs = 59;
							else secs--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
			} else if (pb_status(PB_LEFT) == PB_DOWN){
				pb_wait_any_keyup();
				flagPBS = FALSE;
				for (i = 0; i < MODOS; i++){
					apagaIcono(posiciones[i * 2],posiciones[i * 2 + 1],i,&descripciones[i*5]);
				}
				while ((mins > 0 || secs > 0) && flagPBS==FALSE){
					if (flag1Seg == TRUE){
						if (secs == 0) {
							mins--;
							secs = 59;
						}
						else secs--;
						flag1Seg = FALSE;
						dibujar_x3_Numeros(97,83,BLACK,mins);
						dibujar_x3(145,80,BLACK,":");
						dibujar_x3_Numeros(169,83,BLACK,secs);
					}
				}
				for (i = 0; i < MODOS; i++){
					if (i == modo)  dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],BLACK,i,&descripciones[i*5]);
					else dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],WHITE,i,&descripciones[i*5]);
				}
				flagPBS = FALSE;
				if (mins == 0 && secs == 0){
					mins = 5;
					secs = 0;
					hablar("Temporizador acabado. ");
				}
				else hablar ("Temporizador pausado. ");
			}
			//flagPBS=FALSE;
			dibujar_x3_Numeros(97,83,0x3,mins);
			dibujar_x3(145,80,0x3,":");
			dibujar_x3_Numeros(169,83,0x3,secs);
		}
		comprobarTS();
		if (modo == VOZ_MODO){
			hablar("Temporizador");
			modo = TEMPORIZADOR_MODO;
		}
		comprobarAlarma();
	}

}
void cronometro_modo(void){
	uint8 mins, secs,i;
	mins = 0;
	secs = 0;
	dibujar_x3_Numeros(97,83,0x3,mins);
	dibujar_x3(145,80,0x3,":");
	dibujar_x3_Numeros(169,83,0x3,secs);
	hablar("Cronometro");
	while (modo == CRONOMETRO_MODO){
		if (flagPBS == TRUE){
			if (pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN ){
				flagPBS= FALSE;
				mins = 0;
				secs = 0;
			} else if (pb_status(PB_LEFT) == PB_DOWN){
				pb_wait_any_keyup();
				flagPBS = FALSE;
				for (i = 0; i < MODOS; i++){
					apagaIcono(posiciones[i * 2],posiciones[i * 2 + 1],i,&descripciones[i*5]);
				}
				while ((mins < 100 && secs < 59) && flagPBS==FALSE){
					if (flag1Seg == TRUE){
						if (secs == 59) {
							mins++;
							secs = 0;
						}
						else secs++;
						flag1Seg = FALSE;
						dibujar_x3_Numeros(97,83,BLACK,mins);
						dibujar_x3(145,80,BLACK,":");
						dibujar_x3_Numeros(169,83,BLACK,secs);
					}
				}
				for (i = 0; i < MODOS; i++){
					if (i == modo)  dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],BLACK,i,&descripciones[i*5]);
					else dibujaIcono(posiciones[i * 2],posiciones[i * 2 + 1],WHITE,i,&descripciones[i*5]);
				}
				flagPBS = FALSE;
				hablar("Cronometro acabado en ");
				hablarNumeros(mins);
				hablar(" minutos y ");
				hablarNumeros(secs);
				hablar(" segundos. ");
			}
			flagPBS = FALSE;
			dibujar_x3_Numeros(97,83,0x3,mins);
			dibujar_x3(145,80,0x3,":");
			dibujar_x3_Numeros(169,83,0x3,secs);
		}
		comprobarTS();
		if (modo == VOZ_MODO){
			hablar("Cronometro");
			modo = CRONOMETRO_MODO;
		}
		comprobarAlarma();
	}

}
void hora_modo(void){
	boolean siguiente = FALSE;
	dibujarInterfaz();
	rtc_time_t aux = rtc_time;
	hablar("Son las");
	dibujar_x3_Numeros(97,83,BLACK,rtc_time.hour);
	hablarNumeros(rtc_time.hour);
	dibujar_x3(145,83,BLACK,":");
	hablar("horas y ");
	dibujar_x3_Numeros(169,83,BLACK,rtc_time.min);
	hablarNumeros(rtc_time.min);
	hablar(" minutos.");
	while (modo == HORA_MODO){
		dibujar_x3_Numeros(97,83,BLACK,rtc_time.hour);
		dibujar_x3(145,80,BLACK,":");
		dibujar_x3_Numeros(169,83,BLACK,rtc_time.min);
		if (flag1Seg == TRUE){
			aux = rtc_time;
			flag1Seg = FALSE;
		}
		if (flagPBS == TRUE){
			if (pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN ){
				flagPBS= FALSE;
				dibujar_x3_Numeros(169,83,0x2,aux.min);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(97,83,BLACK,aux.hour);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.hour = (aux.hour + 1) % 24;
						else if (pb_status(PB_LEFT) == PB_DOWN) {
							if (aux.hour == 0) aux.hour = 23;
							else aux.hour--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				dibujar_x3_Numeros(97,83,0x2,aux.hour);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(169,83,BLACK,aux.min);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.min = (aux.min + 1) % 60;
						else if (pb_status(PB_LEFT) == PB_DOWN){
							if (aux.min == 0) aux.min = 59;
							else aux.min--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				rtc_puttime(&aux);
				dibujar_x3_Numeros(97,83,BLACK,rtc_time.hour);
				dibujar_x3_Numeros(169,83,BLACK,rtc_time.min);
			} else flagPBS = FALSE;
		}
		comprobarTS();
		if (modo == VOZ_MODO){
			hablar("Son las");
			hablarNumeros(rtc_time.hour);
			hablar("horas y ");
			hablarNumeros(rtc_time.min);
			hablar(" minutos.");
			modo = HORA_MODO;
		}
		comprobarAlarma();
	}
}
void fecha_modo(void){
	boolean siguiente = FALSE;
	rtc_time_t aux = rtc_time;
	hablar("Hoy es el");
	dibujar_x3_Numeros(67,83,BLACK,rtc_time.mday);
	hablarNumeros(rtc_time.mday);
	dibujar_x3(115,83,BLACK,"/");
	hablar(" del mes ");
	dibujar_x3_Numeros(139,83,BLACK,rtc_time.mon);
	hablarNumeros(rtc_time.mon);
	dibujar_x3(187,83,BLACK,"/");
	hablar(" del año ");
	dibujar_x3_Numeros(211,83,BLACK,rtc_time.year);
	hablarNumeros(rtc_time.year);

	while (modo == FECHA_MODO){
		dibujar_x3_Numeros(67,83,BLACK,rtc_time.mday);
		dibujar_x3(115,83,BLACK,"/");
		dibujar_x3_Numeros(139,83,BLACK,rtc_time.mon);
		dibujar_x3(187,83,BLACK,"/");
		dibujar_x3_Numeros(211,83,BLACK,rtc_time.year);
		if (flag1Seg == TRUE){
			aux = rtc_time;
			flag1Seg = FALSE;
		}
		if (flagPBS == TRUE){
			if (pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN ){
				flagPBS= FALSE;
				dibujar_x3_Numeros(139,83,0x2,aux.mon);
				dibujar_x3_Numeros(211,83,0x2,aux.year);
				while (siguiente == FALSE){									// CONFIGURAMOS EL DIA
					dibujar_x3_Numeros(67,83,BLACK,aux.mday);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.mday = (aux.mday + 1) % 32;
						else if (pb_status(PB_LEFT) == PB_DOWN) {
							if (aux.mday == 0) aux.mday = 31 ;
							else aux.mday--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				dibujar_x3_Numeros(67,83,0x2,aux.mday);
				dibujar_x3_Numeros(211,83,0x2,aux.year);
				while (siguiente == FALSE){								// CONFIGURAMOS EL MES
					dibujar_x3_Numeros(139,83,BLACK,aux.mon);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.mon = (aux.mon + 1) % 13;
						else if (pb_status(PB_LEFT) == PB_DOWN){
							if (aux.mon == 0) aux.mon = 12;
							else aux.mon--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				dibujar_x3_Numeros(67,83,0x2,aux.mday);
				dibujar_x3_Numeros(139,83,0x2,aux.mon);
				while (siguiente == FALSE){							// CONFIGURAMOS EL ANO
					dibujar_x3_Numeros(211,83,BLACK,aux.year);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.year = (aux.year + 1) % 100;
						else if (pb_status(PB_LEFT) == PB_DOWN){
							if (aux.year == 0) aux.year = 99;
							else aux.year--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				rtc_puttime(&aux);
				dibujar_x3_Numeros(67,83,BLACK,aux.mday);
				dibujar_x3_Numeros(139,83,BLACK,aux.mon);
				dibujar_x3_Numeros(211,83,BLACK,aux.year);
			}else flagPBS = FALSE;
		}
		comprobarTS();
		if (modo == VOZ_MODO){
			hablar("Hoy es el");
			hablarNumeros(rtc_time.mday);
			hablar(" del mes ");
			hablarNumeros(rtc_time.mon);
			hablar(" del año ");
			hablarNumeros(rtc_time.year);
			modo = FECHA_MODO;
		}
		comprobarAlarma();
	}
}
void alarma_modo(void){
	rtc_time_t aux = rtc_alarma;
	boolean siguiente = FALSE;
	dibujar_x3_Numeros(97,83,BLACK,rtc_alarma.hour);
	dibujar_x3(145,80,BLACK,":");
	dibujar_x3_Numeros(169,83,BLACK,rtc_alarma.min);
	if (alarmaActiva == FALSE){
		hablar("Alarma no programada. ");
	}
	else {
		hablar("Alarma programada a las ");
		hablarNumeros(rtc_alarma.hour);
		hablar("horas y ");
		hablarNumeros(rtc_alarma.min);
		hablar("minutos. ");
	}
	while (modo == ALARMA_MODO){
		if (flagPBS == TRUE){
			if (pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN){
				flagPBS= FALSE;
				dibujar_x3_Numeros(169,83,0x2,aux.min);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(97,83,BLACK,aux.hour);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.hour = (aux.hour + 1) % 24;
						else if (pb_status(PB_LEFT) == PB_DOWN) {
							if (aux.hour == 0) aux.hour = 23;
							else aux.hour--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				dibujar_x3_Numeros(97,83,0x2,aux.hour);
				while (siguiente == FALSE){
					dibujar_x3_Numeros(169,83,BLACK,aux.min);
					if (flagPBS == TRUE){
						if(pb_status(PB_RIGHT) == PB_DOWN && pb_status(PB_LEFT) == PB_DOWN) siguiente = TRUE;
						else if (pb_status(PB_RIGHT) == PB_DOWN) aux.min = (aux.min + 1) % 60;
						else if (pb_status(PB_LEFT) == PB_DOWN){
							if (aux.min == 0) aux.min = 59;
							else aux.min--;
						}
						flagPBS = FALSE;
					}
				}
				siguiente = FALSE;
				rtc_alarma = aux;
			}
			if (pb_status(PB_LEFT) == PB_DOWN) {
					if(alarmaActiva == TRUE){
						alarmaActiva = FALSE;
						hablar("Alarma desactivada. ");
					}
					else {
						alarmaActiva = TRUE;
						hablar("Alarma activada. ");
					}
					dibujarInterfaz();
			}
			flagPBS=FALSE;
		}
		dibujar_x3_Numeros(97,83,BLACK,rtc_alarma.hour);
		dibujar_x3(145,80,BLACK,":");
		dibujar_x3_Numeros(169,83,BLACK,rtc_alarma.min);
		comprobarTS();
		if (modo == VOZ_MODO){
			if (alarmaActiva == FALSE){
				hablar("Alarma no programada. ");
			}
			else {
				hablar("Alarma programada a las ");
				hablarNumeros(rtc_alarma.hour);
				hablar("horas y ");
				hablarNumeros(rtc_alarma.min);
				hablar("minutos. ");
			}
			modo = ALARMA_MODO;
		}
		comprobarAlarma();
	}
}
void voz_modo(void){
	/*Esta funcion se implementa en cada modo ya que es un pseudomodo */
}
