#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "Timer0.h"

static volatile uint16_t mSecCnt = 0;


void Timer0_Ini() {
	
	/* Timer0 Modo CTC, TOP = OCRA */
	TCCR0A = 1 << WGM01;
	
	/* Ticks [Ticks < 256] ~ ((TiempoDeseado * F_CPU) / (Preescalador)) */
	TCCR0B = 3 << CS00; /* Preescalador = 64 */
	
	/* Asignamos tope (valor - 1) */
	OCR0A = 250 - 1;
	
	/* Inicializamos el contador */
	TCNT0 = 0;
	
	/* Habilitamos interrupcion al comparar con OCRA, el vector de interrupcion ahora sera TIMER0_COMPA_vect */
	TIMSK0 = 1 << OCIE0A;
	
	/* Habilitamos interrupciones */
	sei(); 
}


void setTime() {
	mSecCnt = 0;
}

uint16_t getTime() {
	return mSecCnt;
}

ISR( TIMER0_COMPA_vect ) {
	mSecCnt++; /* Incrementa contador de milisegundos */
}