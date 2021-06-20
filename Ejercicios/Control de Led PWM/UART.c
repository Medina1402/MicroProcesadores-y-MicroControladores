#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"

/* Variables globales */
ring_buffer_t bufferTX[4]; /* Buffer de transmision */
ring_buffer_t bufferRX[4]; /* Buffer de recepcion */

/*
 * Inicializa el perif?rico del UART en un esquema de interrupciones.
 * ** Parametos **
 * > com: representa el n?mero de UART a configurar.
 * > baudrate: representa la velocidad en Baud de configuraci?n.
 * > size: representa el n?mero de bits de los datos con los que operar? el UARTx
 * > parity: representa el tipo de paridad con los que operar? el UARTx
 * > stop: representa el n?mero de bits de paro con los que operar? el UARTx
*/
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop) {
    /* Seleccion de UART - USART Control and Status Register n X */
    volatile uint8_t * UCSRnX = &UCSR0A;
    if(com==3) UCSRnX = &UCSR3A;
    else UCSRnX += (com*8);

    /* Deshabilitar interrupciones globales */
    cli();

    /* Asegurarnos de utilizar modo Asincrono */
    *(UCSRnX + UCSRC) &= ~((1 << UMSEL01) | (1 << UMSEL00));

    /* Habilitamos Recepcion y Transmision e Interrupcion de Recepcion Completada */
    *(UCSRnX + UCSRB) |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

    /* Baudrate, Velocidad en baud */
    *(UCSRnX + UBRRH) = (uint8_t) UBRR(baudrate) >> 8;
    *(UCSRnX + UBRRL) = (uint8_t) UBRR(baudrate);

    /* Tamano de los datos */
    if(size == 9) {
        *(UCSRnX + UCSR0B) |= (1 << UCSZ02); /* Habilita el bit para el 9no*/
        size--;  /* ndatos = 8, para habilitar UCSZn1|0 */
    }
    *(UCSRnX + UCSRC) |= (((size-5)&3) << UCSZ00); /* 5,6,7,8 */

    /* Determinar paridad, 1 = impar, 2 = par */
    if(parity) {
        if(parity == 2) *(UCSRnX + UCSRC) |= (1 << UPM01);
        else *(UCSRnX + UCSRC) |= (1 << UPM01) | (1 << UPM00);
    }

    /* Bit de paro */
    *(UCSRnX + UCSRC) |= (((stop-1)&1) << USBS0);

    /* Inicializar buffers */
    bufferRX[com].out_idx = 0;
    bufferRX[com].in_idx = 0;
    bufferTX[com].out_idx = 0;
    bufferTX[com].in_idx = 0;

    /* Habilitar interrupciones globales  */
    sei();
}


/**
 *
*/
void UART_Ini_UBRR(uint16_t ubrr) {
	volatile uint8_t * UCSRnX = &UCSR0A;
	UART_Ini(0, 0, 8, 0, 1);
	
	cli();
	*(UCSRnX + UBRRH) = (uint8_t) ubrr >> 8;
	*(UCSRnX + UBRRL) = (uint8_t) ubrr;
	sei();
}

/*
 * Funci?n que coloca el dato en la cola circular
*/
void UART_putchar(uint8_t com, char data) {
    volatile uint8_t * UCSRnX = &UCSR0A;
    if(com==3) UCSRnX = &UCSR3A;
    else UCSRnX += (com*8);

    /* Esperamos a que el buffer de transmision tenga un espacio */
    while(BUFFER_FULL(bufferTX[com]));

    /* Introducimos el dato en el buffer de transmision */
    bufferTX[com].buffer[bufferTX[com].in_idx] = data;
    bufferTX[com].in_idx = MOD((bufferTX[com].in_idx+1));

    /* Habilitar interrupcion de registro de datos vacio  */
    *(UCSRnX + UCSRB) |= (1<<UDRIE0);
}

/*
 * Funci?n toma el dato correspondiente a salir de la cola circular.
 * Esto si existe alguno dato en la cola circular, de lo contrario espera a que exista
 * uno para tomarlo y retornarlo
*/
char UART_getchar(uint8_t com) {
    volatile uint8_t * UCSRnX = &UCSR0A;
    if(com==3) UCSRnX = &UCSR3A;
    else UCSRnX += (com*8);
    uint8_t caracter;

    /* Esperamos a que el buffer no este vacio */
    while(!UART_available(com));

    /* Extraemos el dato del buffer de recepcion */
    caracter = bufferRX[com].buffer[bufferRX[com].out_idx];
    bufferRX[com].out_idx = MOD((bufferRX[com].out_idx+1));
	UART_putchar(com, caracter);

    return caracter;
}

/*
 * Funci?n que retorna 1 si existe(n) dato(s) en la cola circular
*/
uint8_t UART_available(uint8_t com) {
    return !BUFFER_EMPTY(bufferRX[com]);
}

/*
 * Imprime una cadena mediante UART_putchar(uint8_t com, char data)
*/
void UART_puts(uint8_t com, char *str) {
    while(*str) UART_putchar(com, *(str++));
}

/*
 * Retorna una cadena haciendo uso de UART_getchar(uint8_t com), la cadena se
 * retorna en el apuntador str
*/
void UART_gets(uint8_t com, char *str) {
    static const uint8_t MAXSIZE = 20;
    char * aux = str;
    char caracter;

    /* Capturamos el primer caracter */
    caracter = UART_getchar(com);

    while(caracter !=  ASCII_ENTER) {
        if(caracter != ASCII_BACKSPACE) {
            if(str < (aux + MAXSIZE - 1)) {
                *(str++) = caracter;
            } else {
                UART_putchar(com, ASCII_BACKSPACE);
                UART_putchar(com, ASCII_ESPACE);
                UART_putchar(com, ASCII_BACKSPACE);
            }
        } else {
            if(str == aux) {
                UART_putchar(com, ASCII_ESPACE);
            } else {
                UART_putchar(com, ASCII_ESPACE);
                UART_putchar(com, caracter);
                str--;
            }
        }
        caracter = UART_getchar(com);
    }
    UART_putchar(com, ASCII_NEWLINE);
    *str = ASCII_NUL;
}

/*
 * Limpia la terminal mediante la secuencia de escape
*/
void UART_clrscr(uint8_t com){
    UART_puts(com, "\x1b[2J"); /* Limpia la plantalla */
    UART_puts(com, "\x1b[H");  /* Regresa el cursor al inicio */
}

/*
 * Env?a la secuencia de escape para configurar el color del texto que se
 * desplegar? en la terminal
*/
void UART_setColor(uint8_t com, uint8_t color){
    char str[] = "\x1b[37m";
    str[3] = color + '0';
    UART_puts(com, str);
}

/*
 *  Posiciona el cursor en la terminal en la coordenada x,y que lleguen como
 *  par?metro, utilizando la secuencia de escape
*/
void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y) {
    char str[] = "\x1b[00;00H";
	
	str[6] = x%10 + '0';
	str[3] = y%10 + '0';
	x/=10;
	y/=10;
	str[5] = x%10 + '0';
	str[2] = y%10 + '0';

    UART_puts(com, str);
}

/**/
void itoa(uint16_t number, char* str, uint8_t base) {
    char *str_start = str;
    char aux;

    do {
        aux = '0' + number%base;
        *(str++) = (aux > '9') ?aux+7 :aux;
        number /= base;
    } while(number > 0 );

    for(*(str--)='\0'; str_start < str; str_start++, str--) {
        aux = *str_start;
        *str_start = *str;
        *str = aux;
    }
}

/**/
uint16_t atoi(char *str) {
    uint16_t number = 0;
    uint8_t aux = 0;

    while(*str) {
        aux = *str - '0';
        if(aux >= 0 && aux <= 9) {
            number += aux;
            str++;
            if(aux >= '0' && aux <= '9') number *= 10;
        } else break;
    }

    return number;
}

/* ============================================================== */
/*        Interrupciones USART <UART Sincrono y Asincrono>        */
/* ============================================================== */

/* ----- UART0 ----- */
ISR(USART0_RX_vect) {
        if(!BUFFER_FULL(bufferRX[0])) {
            bufferRX[0].buffer[bufferRX[0].in_idx] = UDR0;
            bufferRX[0].in_idx = MOD((bufferRX[0].in_idx+1));
        }
}

ISR(USART0_UDRE_vect){
        if(BUFFER_EMPTY(bufferTX[0])) {
            UCSR0B &= ~(1 << UDRIE0);
        } else {
            UDR0 = bufferTX[0].buffer[bufferTX[0].out_idx];
            bufferTX[0].out_idx = MOD((bufferTX[0].out_idx+1));
        }
}

/* ----- UART1 ----- */
ISR(USART1_RX_vect) {
        if(!BUFFER_FULL(bufferRX[1])) {
            bufferRX[1].buffer[bufferRX[1].in_idx] = UDR1;
            bufferRX[1].in_idx = MOD((bufferRX[1].in_idx+1));
        }
}

ISR(USART1_UDRE_vect){
        if(BUFFER_EMPTY(bufferTX[1])) {
            UCSR1B &= ~(1 << UDRIE1);
        } else {
            UDR1 = bufferTX[1].buffer[bufferTX[1].out_idx];
            bufferTX[1].out_idx = MOD((bufferTX[1].out_idx+1));
        }
}

/* ----- UART2 ----- */
ISR(USART2_RX_vect) {
        if(!BUFFER_FULL(bufferRX[2])) {
            bufferRX[2].buffer[bufferRX[2].in_idx] = UDR2;
            bufferRX[2].in_idx = MOD((bufferRX[2].in_idx+1));
        }
}

ISR(USART2_UDRE_vect){
        if(BUFFER_EMPTY(bufferTX[2])) {
            UCSR2B &= ~(1 << UDRIE2);
        } else {
            UDR2 = bufferTX[2].buffer[bufferTX[2].out_idx];
            bufferTX[2].out_idx = MOD((bufferTX[2].out_idx+1));
        }
}

/* ----- UART3 ----- */
ISR(USART3_RX_vect) {
        if(!BUFFER_FULL(bufferRX[3])) {
            bufferRX[3].buffer[bufferRX[3].in_idx] = UDR3;
            bufferRX[3].in_idx = MOD((bufferRX[3].in_idx+1));
        }
}

ISR(USART3_UDRE_vect){
        if(BUFFER_EMPTY(bufferTX[3])) {
            UCSR3B &= ~(1 << UDRIE3);
        } else {
            UDR3 = bufferTX[3].buffer[bufferTX[3].out_idx];
            bufferTX[3].out_idx = MOD((bufferTX[3].out_idx+1));
        }
}