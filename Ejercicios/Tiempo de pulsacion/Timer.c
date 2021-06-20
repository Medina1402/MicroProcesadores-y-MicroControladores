#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "Timer.h"

static volatile uint8_t SecFlag;
static volatile uint16_t counter = 0;

void Timer0_Ini() {
	/* Timer0 Modo CTC, TOP = OCRA */
	TCCR0A = 1 << WGM01;
	
	/* Ticks [Ticks < 256] ~ ((TiempoDeseado * F_CPU) / (Preescalador)) */
	TCCR0B = 2 << CS00;
	
	/* Asignamos tope (valor - 1) */
	OCR0A = 200 - 1;
	
	/* Inicializamos el contador */
	TCNT0 = 0;
	
	/* Habilitamos interrupcion al comparar con OCRA, el vector de interrupcion ahora sera TIMER0_COMPA_vect */
	TIMSK0 = 1 << OCIE0A;
	
	/* Habilitamos interrupciones */
	sei();
}

uint8_t Timer0_SecFlag() {
	if(SecFlag) {
		SecFlag = 0;
		return 1;
	}
	return 0;
}

void setTime() {
	counter = 0;
}

uint16_t getTime() {
	return counter;
}

ISR( TIMER0_COMPA_vect ) {
	counter++; /* Incrementa contador de decima de milisegundos */
}