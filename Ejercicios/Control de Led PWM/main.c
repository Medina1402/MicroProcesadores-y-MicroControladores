#include <avr/io.h>
#include "UART.h"

// Modo PWM
void Timer0_Ini() {
	/* Timer0 Modo PWM */
	TCCR0A = 3 << WGM01;
	
	/* Ticks [Ticks < 256] ~ ((TiempoDeseado * F_CPU) / (Preescalador)) */
	TCCR0B = 3 << CS00; /* Preescalador = 64 */
	TCCR0B |= 1 << WGM01;
	
	/* Asignamos tope (valor - 1) */
	OCR0A = 249;
	OCR0B = OCR0A; // PWM = 100%
	
	// Salidas para el LED ; solo el bit0 quedara de salida, los demas de entrada
	/* Se sacrificara OCRxA para usar el PWM y solo sera utilizable la salida de OCRxB */
	DDRH |= (1<<PH6);
	
	/* Inicializamos el contador */
	TCNT0 = 0;
	
	/* Habilitamos interrupciones globales */
	sei();
}


void updateStatusLed(int action) {
	static uint8_t value_led = 100;
	
	if(action == 1 || action == -1) value_led += action;
	
	if (value_led < 0) value_led == 0;
	else if (value_led > 100) value_led == 100;
	
	
	if (value_led > 0 && value_led < 100) OCR0B = OCR0A * (value_led / 100);
	else if (action == 100) OCR0B = OCR0A * (value_led / 100);
	else if (action == 0) OCR0B = 0;
	
}


int main(void) {
	char key_press;
	
	UART_Ini(0, 9600, 8, 0, 1);
	UART_puts(0, "\rPresiona 8 para incrementar");
	UART_puts(0, "\n\rPresiona 2 para decrementar");
	UART_puts(0, "\n\rPresiona para iniciar...");
	UART_getchar(0);
	
	Timer0_Ini();
	
    while (1) {
		
		if(!UART_available(0)) {
			key_press = UART_getchar(0);
			
			switch(key_press) {
				case "8": updateStatusLed(1); break;
				case "2": updateStatusLed(-1); break;
				case "0": updateStatusLed(0); break;
				case "1": updateStatusLed(100); break;
			}
			
		}
		
    }
}

