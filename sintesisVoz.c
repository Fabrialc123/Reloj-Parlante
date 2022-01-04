
#include <iis.h>
#include "sintesisVoz.h"

// Identificadores de sonidos
typedef enum
{
    // vocales (5)
    a, e, i, o, u,
    // consonantes (19)
    b, d, f, g, j, k, l, m, n, N, p, r, R, s, t, x, y, z, H,
    // consontante + vocal (95)
    ba, be, bi, bo, bu, da, de, di, Do, du, fa, fe, fi, fo, fu,
    ga, ge, gi, go, gu, ja, je, ji, jo, ju,    ka, ke, ki, ko, ku,
    la, le, li, lo, lu, ma, me, mi, mo, mu, na, ne, ni, no, nu,
    Na, Ne, Ni, No, Nu, pa, pe, pi, po, pu, ra, re, ri, ro, ru,
    Ra, Re, Ri, Ro, Ru, sa, se, si, so, su, ta, te, ti, to, tu,
    xa, xe, xi, xo, xu, ya, ye, yi, yo, yu,    za, ze, zi, zo, zu,
    Ha, He, Hi, Ho, Hu,
    // trabadas (60)
    bla, ble, bli, blo, blu, bra, bre, bri, bro, bru,
    fla, fle, fli, flo, flu, fra, fre, fri, fro, fru,
    gla, gle, gli, glo, glu, gra, gre, gri, gro, gru,
    kla, kle, kli, klo, klu, kra, kre, kri, kro, kru,
    pla, ple, pli, plo, plu, pra, pre, pri, pro, pru,
    tra, tre, tri, tro, tru,
    dra, dre, dri, dro, dru,
    // mudos (3)
    espacio, coma, punto
} syllableId_t;

// Array de punteros a WAV de sonidos
static const uint8 *syllablesWavFile[punto-a+1] = {
    // vocales (5)
    (uint8 *) 0x0c200000, // a
    (uint8 *) 0x0c208000, // e
    (uint8 *) 0x0c210000, // i
    (uint8 *) 0x0c218000, // o
    (uint8 *) 0x0c220000, // u
    // consonantes (19)
    (uint8 *) 0x0c228000, // b
    (uint8 *) 0x0c230000, // d
    (uint8 *) 0x0c238000, // f
    (uint8 *) 0x0c240000, // g
    (uint8 *) 0x0c248000, // j
    (uint8 *) 0x0c250000, // k
    (uint8 *) 0x0c258000, // l
    (uint8 *) 0x0c260000, // m
    (uint8 *) 0x0c268000, // n
    (uint8 *) 0x0c270000, // N (sonido ñ)
    (uint8 *) 0x0c278000, // p
    (uint8 *) 0x0c280000, // r
    (uint8 *) 0x0c288000, // R (sonido rr)
    (uint8 *) 0x0c290000, // s
    (uint8 *) 0x0c298000, // t
    (uint8 *) 0x0c2a0000, // x
    (uint8 *) 0x0c2a8000, // y
    (uint8 *) 0x0c2b0000, // z
    (uint8 *) 0x0c2b8000, // H (sonido ch)
    // consontante + vocal (95)
    (uint8 *) 0x0c2c0000, // ba (ba y va)
    (uint8 *) 0x0c2c8000, // be (be y ve)
    (uint8 *) 0x0c2d0000, // bi (bi y vi)
    (uint8 *) 0x0c2d8000, // bo (bo y vo)
    (uint8 *) 0x0c2e0000, // bu (bu y vu)
    (uint8 *) 0x0c2e8000, // da
    (uint8 *) 0x0c2f0000, // de
    (uint8 *) 0x0c2f8000, // di
    (uint8 *) 0x0c300000, // Do (do es palabra reservada en C)
    (uint8 *) 0x0c308000, // du
    (uint8 *) 0x0c310000, // fa
    (uint8 *) 0x0c318000, // fe
    (uint8 *) 0x0c320000, // fi
    (uint8 *) 0x0c328000, // fo
    (uint8 *) 0x0c330000, // fu
    (uint8 *) 0x0c338000, // ga
    (uint8 *) 0x0c340000, // ge (sonido gue)
    (uint8 *) 0x0c348000, // gi (sonido gui)
    (uint8 *) 0x0c350000, // go
    (uint8 *) 0x0c358000, // gu (sonido gu y gü)
    (uint8 *) 0x0c360000, // ja
    (uint8 *) 0x0c368000, // je (je y ge)
    (uint8 *) 0x0c370000, // ji (ji y gi)
    (uint8 *) 0x0c378000, // jo
    (uint8 *) 0x0c380000, // ju
    (uint8 *) 0x0c388000, // ka (ka y ca)
    (uint8 *) 0x0c390000, // ke (ke y que)
    (uint8 *) 0x0c398000, // ki (ki y qui)
    (uint8 *) 0x0c3a0000, // ko (ko y co)
    (uint8 *) 0x0c3a8000, // ku (ku y cu)
    (uint8 *) 0x0c3b0000, // la
    (uint8 *) 0x0c3b8000, // le
    (uint8 *) 0x0c3c0000, // li
    (uint8 *) 0x0c3c8000, // lo
    (uint8 *) 0x0c3d0000, // lu
    (uint8 *) 0x0c3d8000, // ma
    (uint8 *) 0x0c3e0000, // me
    (uint8 *) 0x0c3e8000, // mi
    (uint8 *) 0x0c3f0000, // mo
    (uint8 *) 0x0c3f8000, // mu
    (uint8 *) 0x0c400000, // na
    (uint8 *) 0x0c408000, // ne
    (uint8 *) 0x0c410000, // ni
    (uint8 *) 0x0c418000, // no
    (uint8 *) 0x0c420000, // nu
    (uint8 *) 0x0c428000, // Na (sonido ña)
    (uint8 *) 0x0c430000, // Ne (sonido ñe)
    (uint8 *) 0x0c438000, // Ni (sonido ñi)
    (uint8 *) 0x0c440000, // No (sonido ño)
    (uint8 *) 0x0c448000, // Nu (sonido ñu)
    (uint8 *) 0x0c450000, // pa
    (uint8 *) 0x0c458000, // pe
    (uint8 *) 0x0c460000, // pi
    (uint8 *) 0x0c468000, // po
    (uint8 *) 0x0c470000, // pu
    (uint8 *) 0x0c478000, // ra
    (uint8 *) 0x0c480000, // re
    (uint8 *) 0x0c488000, // ri
    (uint8 *) 0x0c490000, // ro
    (uint8 *) 0x0c498000, // ru
    (uint8 *) 0x0c4a0000, // Ra (sonido rra)
    (uint8 *) 0x0c4a8000, // Re (sonido rre)
    (uint8 *) 0x0c4b0000, // Ri (sonido rri)
    (uint8 *) 0x0c4b8000, // Ro (sonido rro)
    (uint8 *) 0x0c4c0000, // Ru (sonido rru)
    (uint8 *) 0x0c4c8000, // sa
    (uint8 *) 0x0c4d0000, // se
    (uint8 *) 0x0c4d8000, // si
    (uint8 *) 0x0c4e0000, // so
    (uint8 *) 0x0c4e8000, // su
    (uint8 *) 0x0c4f0000, // ta
    (uint8 *) 0x0c4f8000, // te
    (uint8 *) 0x0c500000, // ti
    (uint8 *) 0x0c508000, // to
    (uint8 *) 0x0c510000, // tu
    (uint8 *) 0x0c518000, // xa
    (uint8 *) 0x0c520000, // xe
    (uint8 *) 0x0c528000, // xi
    (uint8 *) 0x0c530000, // xo
    (uint8 *) 0x0c538000, // xu
    (uint8 *) 0x0c540000, // ya (ya y lla)
    (uint8 *) 0x0c548000, // ye (ye y lle)
    (uint8 *) 0x0c550000, // yi (yi y lli)
    (uint8 *) 0x0c558000, // yo (yo y llo)
    (uint8 *) 0x0c560000, // yu (yu y llu)
    (uint8 *) 0x0c568000, // za
    (uint8 *) 0x0c570000, // ze (ze y ce)
    (uint8 *) 0x0c578000, // zi (zi y ci)
    (uint8 *) 0x0c580000, // zo
    (uint8 *) 0x0c588000, // zu
    (uint8 *) 0x0c590000, // Ha (sonido cha)
    (uint8 *) 0x0c598000, // He (sonido che)
    (uint8 *) 0x0c5a0000, // Hi (sonido chi)
    (uint8 *) 0x0c5a8000, // Ho (sonido cho)
    (uint8 *) 0x0c5b0000, // Hu (sonido chu)
    // trabadas (60)
    (uint8 *) 0x0c5b8000, // bla
    (uint8 *) 0x0c5c0000, // ble
    (uint8 *) 0x0c5c8000, // bli
    (uint8 *) 0x0c5d0000, // blo
    (uint8 *) 0x0c5d8000, // blu
    (uint8 *) 0x0c5e0000, // bra
    (uint8 *) 0x0c5e8000, // bre
    (uint8 *) 0x0c5f0000, // bri
    (uint8 *) 0x0c5f8000, // bro
    (uint8 *) 0x0c600000, // bru
    (uint8 *) 0x0c608000, // fla
    (uint8 *) 0x0c610000, // fle
    (uint8 *) 0x0c618000, // fli
    (uint8 *) 0x0c620000, // flo
    (uint8 *) 0x0c628000, // flu
    (uint8 *) 0x0c630000, // fra
    (uint8 *) 0x0c638000, // fre
    (uint8 *) 0x0c640000, // fri
    (uint8 *) 0x0c648000, // fro
    (uint8 *) 0x0c650000, // fru
    (uint8 *) 0x0c658000, // gla
    (uint8 *) 0x0c660000, // gle
    (uint8 *) 0x0c668000, // gli
    (uint8 *) 0x0c670000, // glo
    (uint8 *) 0x0c678000, // glu
    (uint8 *) 0x0c680000, // gra
    (uint8 *) 0x0c688000, // gre
    (uint8 *) 0x0c690000, // gri
    (uint8 *) 0x0c698000, // gro
    (uint8 *) 0x0c6a0000, // gru
    (uint8 *) 0x0c6a8000, // kla (sonido cla)
    (uint8 *) 0x0c6b0000, // kle (sonido cle)
    (uint8 *) 0x0c6b8000, // kli (sonido cli)
    (uint8 *) 0x0c6c0000, // klo (sonido clo)
    (uint8 *) 0x0c6c8000, // klu (sonido clu)
    (uint8 *) 0x0c6d0000, // kra (sonido cra)
    (uint8 *) 0x0c6d8000, // kre (sonido cre)
    (uint8 *) 0x0c6e0000, // kri (sonido cri)
    (uint8 *) 0x0c6e8000, // kro (sonido cro)
    (uint8 *) 0x0c6f0000, // kru (sonido kru)
    (uint8 *) 0x0c6f8000, // pla
    (uint8 *) 0x0c700000, // ple
    (uint8 *) 0x0c708000, // pli
    (uint8 *) 0x0c710000, // plo
    (uint8 *) 0x0c718000, // plu
    (uint8 *) 0x0c720000, // pra
    (uint8 *) 0x0c728000, // pre
    (uint8 *) 0x0c730000, // pri
    (uint8 *) 0x0c738000, // pro
    (uint8 *) 0x0c740000, // pru
    (uint8 *) 0x0c748000, // tra
    (uint8 *) 0x0c750000, // tre
    (uint8 *) 0x0c758000, // tri
    (uint8 *) 0x0c760000, // tro
    (uint8 *) 0x0c768000, // tru
    (uint8 *) 0x0c770000, // dra
    (uint8 *) 0x0c778000, // dre
    (uint8 *) 0x0c780000, // dri
    (uint8 *) 0x0c788000, // dro
    (uint8 *) 0x0c790000, // dru
    // mudos (3)
    (uint8 *) 0x0c798000, // espacio
    (uint8 *) 0x0c7a0000, // coma
    (uint8 *) 0x0c7a8000  // punto
};

static void faseMonolitera( char *txtIn, char *txtOut );
static void faseBilitera( char *txtIn, char *txtOut );
static char *getSyllableId( char *txt, syllableId_t *id );
static boolean esPuntuacion( char ch );
static boolean esVocal( char ch );
static boolean esConsonante( char ch );
static boolean esBFGKP( char ch );
static boolean esBDFGKPT( char ch );

/*******************************************************************/

void iis_textToSpeech( char *texto )
{
    char *tp;
    syllableId_t i;

    char textoSimplificado[TXT_MAXLEN];
    char textoFonetico[TXT_MAXLEN];

    faseMonolitera( texto, textoSimplificado );         // Transforma el texto para que use un conjunto reducido de caracteres
    faseBilitera( textoSimplificado, textoFonetico );   // Transforma el texto para que solo tenga
    tp = textoFonetico;

    while( (tp = getSyllableId( tp, &i )) ) {           // Extrae una sílaba del texto y devuelve su identificador de sonido
        iis_playwawFile( syllablesWavFile[i] );        // Reproduce el correspondiente sonido indexando el array de punteros a WAV con el identificador de sonido
        //iis_continue();
    }
}

/*******************************************************************/

/*
Realiza las siguientes transformaciones:
    - Reemplaza letras mayúsculas por sus correspondientes minúsculas
    - Reemplaza vocales acentuadas por sus correspondientes vocales sin acentuar
    - Reemplaza los caracteres no pronunciables por '-'
    - Reemplaza 'v' por 'b'
    - Reemplaza signos de puntuación por '.' o ','
    - Deja sin alterar letras minúsculas, dígitos y el caracter ' '
El texto de salida tiene el mismo número de caracteres que el texto de entrada
*/
static void faseMonolitera( char *txtIn, char *txtOut )
{
    static const char charMap[256] = {// CharMap de ISO-859-1
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//0-15
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//16-31
        ' ', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,',', '-', '.', '-',		//32-47
        '0', '1', '2', '3'   ,'4', '5', '6', '7'   ,'8', '9', '.', '.'   ,'-', '-', '-', '-',		//48-63
        '-', 'a', 'b', 'c'   ,'d', 'e', 'f', 'g'   ,'h', 'i', 'j', 'k'   ,'l', 'm', 'n', 'o',		//64-79
        'p', 'q', 'r', 's'   ,'t', 'u', 'b', 'w'   ,'x', 'y', 'z', '-'   ,'-', '-', '-', '-',		//80-95
        '-', 'a', 'b', 'c'   ,'d', 'e', 'f', 'g'   ,'h', 'i', 'j', 'k'   ,'l', 'm', 'n', 'o',		//96-111
        'p', 'q', 'r', 's'   ,'t', 'u', 'b', 'w'   ,'x', 'y', 'z', '-'   ,'-', '-', '-', '-',		//112-127
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//128-143
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//144-159
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//160-175
        '-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-'   ,'-', '-', '-', '-',		//176-191
        'a', 'a', 'a', 'a'   ,'a', 'a', 'a', 'c'   ,'e', 'e', 'e', 'e'   ,'i', 'i', 'i', 'i',		//192-207
        'd', 'ñ', 'o', 'o'   ,'o', 'o', 'o', '-'   ,'-', 'u', 'u', 'u'   ,'ü', 'y', '-', '-',		//208-223
        'a', 'a', 'a', 'a'   ,'a', 'a', 'a', 'c'   ,'e', 'e', 'e', 'e'   ,'i', 'i', 'i', 'i',		//224-239
        '-', 'ñ', 'o', 'o'   ,'o', 'o', 'o', '-'   ,'-', 'u', 'u', 'u'   ,'ü', 'y', '-', 'y'		//240-255
    };
    while( *txtIn )
        *txtOut++ = charMap[(uint8) *txtIn++];
    *txtOut = '\0';
}

/*
Asumiento que es un texto correcto en español, realiza las siguientes transformaciones:
    - Elimina '-' (caracteres no pronunciables)
    - Reemplaza los dígitos por una serie de letras fonéticas
    - Reemplaza "ca", "co", "cu" por "ka", "ko", "ku"
    - Remplaza "ce", "ci" por "ze", "zi"
    - Reemplaza "cr", "cl" por "kr", "kl"
    - Reemplaza "ch" por "H"
    - Reemplaza "gue", "gui" por "ge", "gi"
    - Reemplaza "ge", "gi" por "je", "ji"
    - Reemplaza "gü" por "gu"
    - Elimina "h"
    - Reemplaza "ll" por "y"
    - Reemplaza "que", "qui" por "ke", "ki"
    - Reemplaza "rr" por "R"
    - Reemplaza "y " por "i"
    - Reemplaza " r" por "R"
    - Reemplaza "w" por "gu"
El texto de salida tiene un distinto número de caracteres que el texto de entrada
*/
static void faseBilitera( char *txtIn, char *txtOut )
{
    while( *txtIn )
        switch( txtIn[0] ) {
        case 'ñ':
        	*txtOut++ = 'N';
        	txtIn++;
        	break;
        case 'w':
        	*txtOut++ = 'g';
        	*txtOut++ = 'u';
        	txtIn++;
        	break;
        case ' ':
        	switch(txtIn[1]){
        	case 'r':
        		*txtOut++ = ' ';
        		*txtOut++ = 'R';
        		txtIn += 2;
        		break;
        	default:
        		*txtOut++ = ' ';
        		txtIn++;
        		break;
        	}
        	break;
        case 'y':
        	switch(txtIn[1]){
        	case '.':
        	case ',':
        	case ' ':
        		*txtOut++ = 'i';
        		txtIn ++;
        		break;
        	default:
        		*txtOut++ = 'y';
        		txtIn++;
        		break;
        	}
        	break;
        case 'r':
        	switch(txtIn[1]){
        	case 'r':
        		*txtOut++ = 'R';
        		txtIn += 2;
        		break;
        	default:
        		*txtOut++ = 'r';
        		txtIn++;
        		break;
        	}
        	break;
        case 'q':
        	switch(txtIn[1]){
        	case 'u':
        		switch(txtIn[2]){
        		case 'e':
        		case 'i':
        			*txtOut++ = 'k';
        			txtIn += 2;
        			break;
        		default:
        			*txtOut++ = 'q';
        			txtIn++;
        			break;
        		}
        		break;
        	default:
    			*txtOut++ = 'q';
    			txtIn++;
    			break;
        	}
        	break;
        case 'l':
        	switch(txtIn[1]){
        	case 'l':
        		*txtOut++ = 'y';
        		txtIn += 2;
        		break;
        	default:
        		*txtOut++ = 'l';
        		txtIn++;
        	}
        	break;
        case 'h':
        	txtIn++;
        	break;
        case 'g':
            switch( txtIn[1] ) {
            case 'u':
            	switch( txtIn[2]){
            	case 'e':
            		*txtOut++='g';
            		*txtOut++='e';
            		txtIn += 3;
            		break;
            	case 'i':
            		*txtOut++='g';
            		*txtOut++='i';
            		txtIn +=3;
            		break;
            	default:
            		*txtOut++='g';
            		*txtOut++='u';
            		txtIn += 2;
            		break;
            	}
                break;
            case 'e':
            case 'i':
                *txtOut++ = 'j';
                txtIn++;
                break;
            case 'ü':
            	*txtOut++ = 'g';
            	*txtOut++ = 'u';
            	txtIn += 2;
            	break;
            default:
                *txtOut++ = 'g';
                txtIn++;
            }
            break;
        case '-':
            txtIn++;
            break;
        case '0':
            *txtOut++ = 'z';
            *txtOut++ = 'e';
            *txtOut++ = 'r';
            *txtOut++ = 'o';
            txtIn++;
            break;
        case '1':
            *txtOut++ = 'u';
            *txtOut++ = 'n';
            *txtOut++ = 'o';
            txtIn++;
            break;
        case '2':
            *txtOut++ = 'd';
            *txtOut++ = 'o';
            *txtOut++ = 's';
            txtIn++;
            break;
        case '3':
            *txtOut++ = 't';
            *txtOut++ = 'r';
            *txtOut++ = 'e';
            *txtOut++ = 's';
            txtIn++;
            break;
        case '4':
            *txtOut++ = 'k';
            *txtOut++ = 'u';
            *txtOut++ = 'a';
            *txtOut++ = 't';
            *txtOut++ = 'r';
            *txtOut++ = 'o';
            txtIn++;
            break;
        case '5':
            *txtOut++ = 's';
            *txtOut++ = 'i';
            *txtOut++ = 'n';
            *txtOut++ = 'k';
            *txtOut++ = 'o';
            txtIn++;
            break;
        case '6':
            *txtOut++ = 's';
            *txtOut++ = 'e';
            *txtOut++ = 'i';
            *txtOut++ = 's';
            txtIn++;
            break;
        case '7':
            *txtOut++ = 's';
            *txtOut++ = 'i';
            *txtOut++ = 'e';
            *txtOut++ = 't';
            *txtOut++ = 'e';
            txtIn++;
            break;
        case '8':
            *txtOut++ = 'o';
            *txtOut++ = 'H';
            *txtOut++ = 'o';
            txtIn++;
            break;
        case '9':
            *txtOut++ = 'n';
            *txtOut++ = 'u';
            *txtOut++ = 'e';
            *txtOut++ = 'b';
            *txtOut++ = 'e';
            txtIn++;
            break;
        case 'c':
            switch( txtIn[1] ) {
            case 'a':
            case 'o':
            case 'u':
                *txtOut++ = 'k';
                txtIn++;
                break;
            case 'e':
            case 'i':
                *txtOut++ = 'z';
                txtIn++;
                break;
            case 'h':
                *txtOut++ = 'H';
                txtIn += 2;
                break;
            default:                        // cr, cl
                *txtOut++ = 'k';
                txtIn++;
            }
            break;
        default:
            *txtOut++ = *txtIn++;
        }
    *txtOut = '\0';
}

/*
Asumiento que es un texto correcto en español, extrae una sílaba del texto y devuelve su identificador de sonido
*/
static char *getSyllableId( char *txt, syllableId_t *id )
{
    if( !*txt )
        return NULL;
    else if( esPuntuacion( txt[0] ) ) {                                                  // mudos (3)
        switch( txt[0] ) {
        case ' ': *id = espacio; break;
        case ',': *id = coma; break;
        case '.': *id = punto; break;
        }
        return txt+1;
    } else if( esVocal( txt[0] ) ) {                                                     // vocales (5)
        switch( txt[0] ){
        case 'a': *id = a; break;
        case 'e': *id = e; break;
        case 'i': *id = i; break;
        case 'o': *id = o; break;
        case 'u': *id = u; break;
        }
        return txt+1;
    } else if( esConsonante( txt[0] ) && esVocal( txt[1] ) ) {                           // consontante + vocal (95)
        switch( txt[0] ) {
        case 'b':
            switch( txt[1] ) {
            case 'a': *id = ba; break;
            case 'e': *id = be; break;
            case 'i': *id = bi; break;
            case 'o': *id = bo; break;
            case 'u': *id = bu; break;
            }
            break;
        case 'd':
            switch( txt[1] ) {
            case 'a': *id = da; break;
            case 'e': *id = de; break;
            case 'i': *id = di; break;
            case 'o': *id = Do; break;
            case 'u': *id = du; break;
            }
            break;
        case 'f':
            switch( txt[1] ) {
            case 'a': *id = fa; break;
            case 'e': *id = fe; break;
            case 'i': *id = fi; break;
            case 'o': *id = fo; break;
            case 'u': *id = fu; break;
            }
            break;
        case 'g':
            switch( txt[1] ) {
            case 'a': *id = ga; break;
            case 'e': *id = ge; break;
            case 'i': *id = gi; break;
            case 'o': *id = go; break;
            case 'u': *id = gu; break;
            }
            break;
        case 'j':
            switch( txt[1] ) {
            case 'a': *id = ja; break;
            case 'e': *id = je; break;
            case 'i': *id = ji; break;
            case 'o': *id = jo; break;
            case 'u': *id = ju; break;
            }
            break;
         case 'k':
             switch( txt[1] ) {
             case 'a': *id = ka; break;
             case 'e': *id = ke; break;
             case 'i': *id = ki; break;
             case 'o': *id = ko; break;
             case 'u': *id = ku; break;
             }
             break;
         case 'l':
             switch( txt[1] ) {
             case 'a': *id = la; break;
             case 'e': *id = le; break;
             case 'i': *id = li; break;
             case 'o': *id = lo; break;
             case 'u': *id = lu; break;
             }
             break;
         case 'm':
             switch( txt[1] ) {
             case 'a': *id = ma; break;
             case 'e': *id = me; break;
             case 'i': *id = mi; break;
             case 'o': *id = mo; break;
             case 'u': *id = mu; break;
             }
             break;
          case 'n':
              switch( txt[1] ) {
              case 'a': *id = na; break;
              case 'e': *id = ne; break;
              case 'i': *id = ni; break;
              case 'o': *id = no; break;
              case 'u': *id = nu; break;
              }
              break;
          case 'N':
              switch( txt[1] ) {
              case 'a': *id = Na; break;
              case 'e': *id = Ne; break;
              case 'i': *id = Ni; break;
              case 'o': *id = No; break;
              case 'u': *id = Nu; break;
              }
              break;
          case 'p':
              switch( txt[1] ) {
              case 'a': *id = pa; break;
              case 'e': *id = pe; break;
              case 'i': *id = pi; break;
              case 'o': *id = po; break;
              case 'u': *id = pu; break;
              }
              break;
               case 'r':
                   switch( txt[1] ) {
                   case 'a': *id = ra; break;
                   case 'e': *id = re; break;
                   case 'i': *id = ri; break;
                   case 'o': *id = ro; break;
                   case 'u': *id = ru; break;
                   }
                   break;
              case 'R':
                  switch( txt[1] ) {
                  case 'a': *id = Ra; break;
                  case 'e': *id = Re; break;
                  case 'i': *id = Ri; break;
                  case 'o': *id = Ro; break;
                  case 'u': *id = Ru; break;
                  }
                  break;
              case 's':
                  switch( txt[1] ) {
                  case 'a': *id = sa; break;
                  case 'e': *id = se; break;
                  case 'i': *id = si; break;
                  case 'o': *id = so; break;
                  case 'u': *id = su; break;
                  }
                  break;
              case 't':
                  switch( txt[1] ) {
                  case 'a': *id = ta; break;
                  case 'e': *id = te; break;
                  case 'i': *id = ti; break;
                  case 'o': *id = to; break;
                  case 'u': *id = tu; break;
                  }
                  break;
              case 'x':
                  switch( txt[1] ) {
                  case 'a': *id = xa; break;
                  case 'e': *id = xe; break;
                  case 'i': *id = xi; break;
                  case 'o': *id = xo; break;
                  case 'u': *id = xu; break;
                  }
                  break;
              case 'y':
                  switch( txt[1] ) {
                  case 'a': *id = ya; break;
                  case 'e': *id = ye; break;
                  case 'i': *id = yi; break;
                  case 'o': *id = yo; break;
                  case 'u': *id = yu; break;
                  }
                  break;
              case 'z':
                  switch( txt[1] ) {
                  case 'a': *id = za; break;
                  case 'e': *id = ze; break;
                  case 'i': *id = zi; break;
                  case 'o': *id = zo; break;
                  case 'u': *id = zu; break;
                  }
                  break;
              case 'H':
                  switch( txt[1] ) {
                  case 'a': *id = Ha; break;
                  case 'e': *id = He; break;
                  case 'i': *id = Hi; break;
                  case 'o': *id = Ho; break;
                  case 'u': *id = Hu; break;
                  }
                  break;
        }
        return txt+2;
    } else if( esBFGKP( txt[0] ) && txt[1] == 'l' && esVocal( txt[2] ) ) {               // trabadas con l (25)
    	switch( txt[0] ){
    	case 'b':
    		switch(txt[2]){
            case 'a': *id = bla; break;
            case 'e': *id = ble; break;
            case 'i': *id = bli; break;
            case 'o': *id = blo; break;
            case 'u': *id = blu; break;
    		}
    		break;
    	case 'f':
    		switch(txt[2]){
            case 'a': *id = fla; break;
            case 'e': *id = fle; break;
            case 'i': *id = fli; break;
            case 'o': *id = flo; break;
            case 'u': *id = flu; break;
    		}
    		break;

    	case 'g':
    		switch(txt[2]){
            case 'a': *id = gla; break;
            case 'e': *id = gle; break;
            case 'i': *id = gli; break;
            case 'o': *id = glo; break;
            case 'u': *id = glu; break;
    		}
    		break;
    	case 'k':
    		switch(txt[2]){
            case 'a': *id = kla; break;
            case 'e': *id = kle; break;
            case 'i': *id = kli; break;
            case 'o': *id = klo; break;
            case 'u': *id = klu; break;
    		}
    		break;
    	case 'p':
    		switch(txt[2]){
            case 'a': *id = pla; break;
            case 'e': *id = ple; break;
            case 'i': *id = pli; break;
            case 'o': *id = plo; break;
            case 'u': *id = plu; break;
    		}
    		break;
    	}
        return txt+3;
    }  else if( esBDFGKPT( txt[0] ) && txt[1] == 'r' && esVocal( txt[2] ) ) {            // trabadas con r (35)
    	switch( txt[0] ){
    	case 'b':
    		switch(txt[2]){
            case 'a': *id = bra; break;
            case 'e': *id = bre; break;
            case 'i': *id = bri; break;
            case 'o': *id = bro; break;
            case 'u': *id = bru; break;
    		}
    		break;
    	case 'f':
    		switch(txt[2]){
            case 'a': *id = fra; break;
            case 'e': *id = fre; break;
            case 'i': *id = fri; break;
            case 'o': *id = fro; break;
            case 'u': *id = fru; break;
    		}
    		break;

    	case 'g':
    		switch(txt[2]){
            case 'a': *id = gra; break;
            case 'e': *id = gre; break;
            case 'i': *id = gri; break;
            case 'o': *id = gro; break;
            case 'u': *id = gru; break;
    		}
    		break;
    	case 'k':
    		switch(txt[2]){
            case 'a': *id = kra; break;
            case 'e': *id = kre; break;
            case 'i': *id = kri; break;
            case 'o': *id = kro; break;
            case 'u': *id = kru; break;
    		}
    		break;
    	case 'p':
    		switch(txt[2]){
            case 'a': *id = pra; break;
            case 'e': *id = pre; break;
            case 'i': *id = pri; break;
            case 'o': *id = pro; break;
            case 'u': *id = pru; break;
    		}
    		break;
    	case 't':
    		switch(txt[2]){
            case 'a': *id = tra; break;
            case 'e': *id = tre; break;
            case 'i': *id = tri; break;
            case 'o': *id = tro; break;
            case 'u': *id = tru; break;
    		}
    		break;
    	case 'd':
    		switch(txt[2]){
            case 'a': *id = dra; break;
            case 'e': *id = dre; break;
            case 'i': *id = dri; break;
            case 'o': *id = dro; break;
            case 'u': *id = dru; break;
    		}
    		break;
    	}
        return txt+3;
    } else {                                                                             // consonantes (19)
        switch( txt[0] ) {
        case 'b': *id = b; break;
        case 'd': *id = d; break;
        case 'f': *id = f; break;
        case 'g': *id = g; break;
        case 'j': *id = j; break;
        case 'k': *id = k; break;
        case 'l': *id = l; break;
        case 'm': *id = m; break;
        case 'n': *id = n; break;
        case 'N': *id = N; break;
        case 'p': *id = p; break;
        case 'r': *id = r; break;
        case 'R': *id = R; break;
        case 's': *id = s; break;
        case 't': *id = t; break;
        case 'x': *id = x; break;
        case 'y': *id = y; break;
        case 'z': *id = z; break;
        case 'H': *id = H; break;
        }
        return txt+1;
    }
}

static boolean esPuntuacion( char ch )
{
    switch( ch ) {
    case ' ':
    case ',':
    case '.':
        return TRUE;
    default:
        return FALSE;
    }
}

static boolean esVocal( char ch )
{
    switch( ch ) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return TRUE;
    default:
        return FALSE;
    }
}

static boolean esConsonante( char ch )
{
    switch( ch ) {
    case 'b':
    case 'd':
    case 'f':
    case 'g':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'N':
    case 'p':
    case 'r':
    case 'R':
    case 's':
    case 't':
    case 'x':
    case 'y':
    case 'z':
    case 'H':
        return TRUE;
    default:
        return FALSE;
    }
}

static boolean esBFGKP( char ch )
{
	switch( ch ){
  	case 'b':
    case 'f':
    case 'g':
    case 'k':
    case 'p':
    	return TRUE;
    default:
    	return FALSE;
	}
}

static boolean esBDFGKPT( char ch )
{
	switch( ch ){
  	case 'b':
    case 'f':
    case 'g':
    case 'k':
    case 'p':
    case 't':
    case 'd':
    	return TRUE;
    default:
    	return FALSE;
	}
}

