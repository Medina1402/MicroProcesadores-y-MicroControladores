#ifndef UART_H_
#define UART_H_

#include <inttypes.h>


/* Colores ANSI */
#define BLACK    0
#define RED      1
#define GREEN    2
#define YELLOW   3
#define BLUE     4
#define MAGENTA  5
#define CYAN     6
#define WHITE    7


/* Teclas ASCII */
#define ASCII_NUL        0
#define ASCII_BACKSPACE  8
#define ASCII_NEWLINE    10
#define ASCII_ENTER      13
#define ASCII_ESPACE     32


/* Buffer y BPS */
#define BUFFER_SIZE 64
#define FOSC 16000000
#define UBRR(baudrate) ((FOSC/16/baudrate))
#define SELECT_COM_UART(com) ((com==3)?(&UCSR3A):(&(UCSR0A+(8*com))))


/* Buffer circular */
#define MOD(idx) (idx&(BUFFER_SIZE-1))
#define BUFFER_EMPTY(buffer) (buffer.in_idx==buffer.out_idx)
#define BUFFER_FULL(buffer) (buffer.in_idx+1==buffer.out_idx)
typedef struct {
	char buffer[BUFFER_SIZE]; /* espacio reservado */
	volatile unsigned char in_idx; /* indice entrada (Head) */
	volatile unsigned char out_idx; /* indice entrada (tail) */
} ring_buffer_t;


/* Desplazamientos generales */
#define UCSRA 0
#define UCSRB 1
#define UCSRC 2
#define UBRRL 4
#define UBRRH 5


/**/
void UART_Ini_UBRR(uint16_t ubrr);
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);
void UART_putchar(uint8_t com, char data);
char UART_getchar(uint8_t com);
uint8_t UART_available(uint8_t com);
void UART_puts(uint8_t com, char *str);
void UART_gets(uint8_t com, char *str);
void UART_clrscr(uint8_t com);
void UART_setColor(uint8_t com, uint8_t color);
void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y);
void itoa(uint16_t number, char* str, uint8_t base);
uint16_t atoi(char *str);


#endif /* UART_H_ */