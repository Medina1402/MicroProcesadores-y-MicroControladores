/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#include <avr/io.h>
#include "UART.h"
#include "Timer0.h"

#define SIZE_TIMES 16

/* Numeros pseudo  aleatorios */
static long holdrand = 1L;
void srand(unsigned int seed);
int rand();

/* Auxiliares */
char getCharacter();
void printTime(char * message, uint16_t time);
uint8_t NextPromMaxMin(uint8_t * next, uint8_t * counter, uint8_t times[SIZE_TIMES]);


int main(void) {
	uint16_t times[SIZE_TIMES], time_temp;
	uint8_t counter = 0, next = 1;
	char key_press, keyRandom;
	
	srand(32);
	UART_Ini(0, 9600, 8, 0, 1);
	Timer0_Ini();
	
	UART_puts(0, "Presiona una tecla para iniciar...\n\r");
	UART_getchar(0);
	
    while (1) {
		if(next) {
			keyRandom = getCharacter();
			UART_puts(0, "\n\r");
			UART_putchar(0, keyRandom);
			setTime();
			UART_puts(0, " ");
		}
		
		if(getTime() > 2000) {
			next = 1;
			UART_puts(0, "\n\r Tiempo vencido...");
			
		} else {
			next = 0;
			
			if(UART_available(0)) {
				time_temp = getTime();
				key_press = UART_getchar(0);
				
				if(time_temp < 1000) {
					if(key_press == keyRandom) times[counter++] = time_temp;
					else UART_puts(0, "\n\rTecla incorrecta");
				} else UART_puts(0, "\n\rTiempo vencido");
				
				next = 1;
			}
			
		}
		
		NextPromMaxMin(&next, &counter, times);
    }
}


void srand(unsigned int seed) {
	holdrand = (long) seed;
}


int rand()  {
	return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}


char getCharacter() {
	static char characters[26] = "abcdefghijklmnopqrstuvwxyz"; /* pasar a valores numericos de ascii */
	uint8_t rand8 = rand() % 26;
	return characters[rand8];
}


void printTime(char * message, uint16_t time) {
	char timeText[10];
	UART_puts(0, message);
	
	itoa(time, timeText, 10);
	UART_puts(0, timeText);
	
	UART_puts(0, " ms");
}


/* Promedio de mediciones, maximo y minimo */
uint8_t NextPromMaxMin(uint8_t * next, uint8_t * counter, uint8_t times[SIZE_TIMES]) {
	uint16_t time_temp = 0;
	if(counter == 16) {
		counter = 0;
		time_temp = 0;
		
		for(; counter < 16; counter++) {
			time_temp += times[counter];
			
			if(counter == 0) {
				max = times[counter];
				min = times[counter];
			} else {
				if(max < times[counter]) max = times[counter];
				if(min > times[counter]) min = times[counter];
			}
		}
		time_temp /= 16;
		
		printTime("\n\n\r Promedio: ", time_temp);
		printTime("\n\r Valor Maximo: ", max);
		printTime("\n\r Valor Minimo: ", min);
		
		counter = 0;
		next = 1;
		
		/* Espera y limpieza */
		UART_getchar(0);
		UART_clrscr(0);
	}
}
