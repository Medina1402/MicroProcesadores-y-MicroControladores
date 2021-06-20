#ifndef TIMER0_H_
#define TIMER0_H_

#include <inttypes.h>

typedef struct {
	uint16_t msCurrentTime;
	uint16_t msWaitTime;
	uint8_t repeatCounter;
	uint8_t currentRepeat;
	uint8_t enabled: 1;
	uint8_t (*callback)();
} timerReg_t;

/* Funci?n para inicializar el Timer0 y generar */
/* la temporizaci?n de 1 milisegundo. */
void Timer0_Ini();  // [x]

/* Funci?n para verificar bandera del segundo */
uint8_t Timer0_SecFlag();  // [x]

/* Inicializa Timer0 y deja activada la interrupci?n cada milisegundo */
void TimerInit();  // [-]

/* Retorna el conteo actual de milisegundos desde que inici? el sistema */
uint32_t millis();  // [x]

/* Funci?n que espera n-milisegundos especificados por el par?metro de entrada */
void delay(uint16_t msWaitTime);  // [x]

/* Funci?n que registra un apuntador de funci?n, la cual deber? ser invocada al transcurrir los milisegundos dados en msWaitTime. 
   El ?ltimo par?metro es el n?mero de veces que se autoregistra de nuevo el timer, y el valor 0 seria repetir indefinidamente.
   El prop?sito del par?metro del callback is para utilizarlo como identificador de la gota y generalizar la funci?n.*/
void TimerRegister(void (*callback)(uint8_t), uint16_t msWaitTime, uint8_t repeatCounter);  // [x]


#endif /* TIMER0_H_ */