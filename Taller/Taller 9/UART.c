/*
* Created: 05/05/2021
* Author: Abraham Medina Carrillo
* Repository: https://github.com/medina1402 <private>
* Email: abraham.medina.carrillo@uabc.edu.mx
*/
#include <avr/io.h>

/* BPS */
#define FOSC 16000000
#define UBRR(velocidad) ((FOSC/16/velocidad)-1)

/* Desplazamientos generales */
#define UCSRA 0
#define UCSRB 1
#define UCSRC 2
#define UBRRL 4
#define UBRRH 5

/*
* Valida que los parametros se encuentren en el rango determinado por las tablas de configuracion (pag 221)
*  > Datasheet ATmega640/V-1280/V-1281/V-2560/V-2561/V
* return: 0 = parametros no validos, 1 = parametros validos
*/
uint_t UART_Params_Valid(uint8_t com, uint16_t velocidad, uint8_t ndatos, uint8_t paridad, uint8_t bit_paro) {
    if(com < 4) { /* COM = 0, 1, 2, 3 */
        if(ndatos > 4 && ndatos < 10) { /* ndatos = 5, 6, 7, 8, 9 */
            if(paridad < 3) { /* paridad = 0, 1, 2 */
                if(bit_paro > 0 && bit_paro < 3) return 1; /* bit_paro = 1, 2 */
                else return 0;
            } else return 0;
        } else return 0;
    } else return 0;
}

/*
 * Inicializa el periférico del UART en un esquema de interrupciones.
 * ** Parametos **
 * > com: Representa el número de UART a configurar -- considerar 0, 1, 2 y 3.
 * > velocidad: Representa la velocidad en Baud de configuración -- puede ser no estándar.
 * > ndatos: Representa el número de bits de los datos con los que operará el UARTx -- considerar de 5 a 8 bits.
 * > paridad: Representa el tipo de paridad con los que operará el UARTx. --considerar 0: No paridad, 1: impar, 2: par.
 * > bit_paro: Representa el número de bits de paro con los que operará el UARTx. --considerar 1 o 2
*/
uint8_t UART_Config(uint8_t com, uint16_t velocidad, uint8_t ndatos, uint8_t paridad, uint8_t bit_paro) {
    /* Validacion de parametros */
    if(!UART_Params_Valid(com, ndatos, paridad, bit_paro)) return 0;

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

    /* Velocidad en baud */
    *(UCSRnX + UBRRH) = (uint8_t) UBRR(velocidad) >> 8;
    *(UCSRnX + UBRRL) = (uint8_t) UBRR(velocidad);

    /* Tamano de los datos */
    if(ndatos == 9) {
        *(UCSRnX + UCSR0B) |= (1 << UCSZ02); /* Habilita el bit para el 9no*/
        ndatos--;  /* ndatos = 8, para habilitar UCSZn1|0 */
    }
    *(UCSRnX + UCSRC) |= (((ndatos-5)&3) << UCSZ00); /* 5,6,7,8 */

    /* Determinar paridad, 0 = No paridad, 1 = Impar, 2 = Par */
    if(paridad) {
      if(paridad == 1) *(UCSRnX + UCSRC) |= (3 << UPM00); /* Paridad Impar */
      else *(UCSRnX + UCSRC) |= (1 << UPM01);             /* Paridad Par */
    } else *(UCSRnX + UCSRC) &= ~(3 << UPM00);            /* Deshabilita la paridad */

    /* Bit de paro */
    *(UCSRnX + UCSRC) |= (((bit_paro>>1)&1) << USBS0);

    /* Habilitar interrupciones globales  */
    sei();

    /* Configuracion exitosa */
    return 1;
}