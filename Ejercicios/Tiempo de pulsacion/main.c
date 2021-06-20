/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#include <avr/io.h>
#include "UART.h"
#include "Timer.h"


int main(void) {
	char timeText[2][10];
	uint16_t current_time = 0;
	
	UART_Ini(0, 115200, 8, 0, 1);
	Timer0_Ini();
	
    while (1) {
		UART_clrscr(0);
		
		UART_puts(0, "Presiona una tecla para iniciar");
		UART_getchar(0);
		setTime();
		UART_puts(0, "\nPresiona para finalizar...");
		UART_getchar(0);
		
		UART_puts(0, "\nEl tiempo tanscurrido fue de: ");
		current_time = getTime();
		itoa(current_time / 10, timeText[0], 10);
		itoa(current_time % 10, timeText[1], 10);
		
		UART_puts(0, timeText[0]);
		UART_puts(0, ".");
		UART_puts(0, timeText[1]);
		UART_puts(0, " ms\n");
		
		UART_getchar(0);	
    }
}

