#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "Timer0.h"

#define MAX_REG_TIMERS 2
static volatile uint8_t SecFlag;
static volatile uint32_t mseg;
static timerReg_t timers[MAX_REG_TIMERS];

static volatile uint8_t MSegFlag = 0;
static uint8_t indexReg = 0;

void Timer0_Ini() {
	/* Timer0 Modo CTC, TOP = OCRA */
	TCCR0A = 1 << WGM01;
	
	/* Ticks [Ticks < 256] ~ ((TiempoDeseado * F_CPU) / (Preescalador)) */
	TCCR0B = 3 << CS00; /* Preescalador = 64 */
	
	/* Asignamos tope (valor - 1) */
	OCR0A = 249;
	
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

void delay(uint16_t msWaitTime) {
	while ( msWaitTime ) {
		if ( MSegFlag ) {
			msWaitTime--;
			MSegFlag = 0;
		}
	};
}

uint32_t millis() {
	return mseg;
}

void TimerRegister(void (*callback) (uint8_t), uint16_t msWaitTime, uint8_t repeatCounter) {
	timers[indexReg].repeatCounter = repeatCounter;
	timers[indexReg].msWaitTime = msWaitTime;
	timers[indexReg].callback = callback;

	timers[indexReg].msCurrentTime = 0;
	timers[indexReg].currentRepeat = 0;

	timers[indexReg].enabled = 1;
	indexReg++;
}

void TimerInit() {
	Timer0_Ini();
	mseg = 0;
}

ISR( TIMER0_COMPA_vect ) {
	static uint16_t mSecCnt;
	static uint8_t counterReg;

	mSecCnt++; /* Incrementa contador de milisegundos */
	if( mSecCnt == 1000 ) {
		mSecCnt=0;
		SecFlag=1; /* Bandera de Segundos */
	}

	for(counterReg = 0; counterReg < indexReg; counterReg++) {		
		/* El timer estara habilitado siempre que la repeticion actual no supere la limite o que la limite sea 0 */
		if(timers[counterReg].enabled) {
			/* El intervalo de tiempo para el timer actual transcurrio */
			if(timers[counterReg].msCurrentTime >= timers[counterReg].msWaitTime) {
				timers[counterReg].msCurrentTime = 0; /* Limpieza para un nuevo conteo */
				/* Si la repeticionesta en 0, son repeticiones infinitas */
				if(timers[counterReg].repeatCounter) { /* Repeticiones limitadas */
					timers[counterReg].callback();
					timers[counterReg].currentRepeat++;
					/* Repeticion actual es mayor o igual al limite, deshabilitamos el timer actual */
					if(timers[counterReg].currentRepeat >= timers[counterReg].repeatCounter) {
						timers[counterReg].enabled = 0;
					}
				} else timers[counterReg].callback(); /* Repeticiones infinitas */
			} else timers[counterReg].msCurrentTime++;
		}
	}
	mseg++; /* Contador global de milisegundos */
	MSegFlag = 1; /* Delay de Milisegundos */
}