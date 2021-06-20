/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#include <avr/io.h>
#include "UART.h"

/* Prototipos externos, ASM */
extern uint8_t myRand(uint8_t seed);
extern void delay(uint16_t mseg);

/* Constantes */
#define SEED     37
#define MAX_LONG 8
#define SIZE_W   80
#define SIZE_H   24

/* Caracteres disponibles, para imprimir los caracteres o simbolos deseados */
#define SIZE_CARACTERS 26
volatile uint8_t caracteres[SIZE_CARACTERS] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* Variables globales */
volatile uint32_t timer;
volatile uint8_t temp_row;

/* Definicion de propiedades y caracteristicas de la gota */
typedef struct {
	uint8_t index_current;    /* Valor de indice maximo que se imprimira (este es de color distinto) */
	uint8_t values[MAX_LONG]; /* Contendra los valores aleatorios que perteneceran a la gota */
	uint8_t global_pos_y;     /* Posicion vertical del primer caracter a imprimir en la pantalla */
	uint8_t global_pos_x;     /* Posicion horizontal del caracter a imprimir en la pantalla */
	uint16_t sleep_timer;     /* Contador, si es igual a "timer" activara "draw"*/
	uint8_t draw: 1;          /* Indicara cuanto debe comenzar a dibujarse la gota <solo necesitamos 1 bit> */
} drop_t;

/* Prototipos internos */
drop_t createDrop(uint8_t posX);
void drawDrop(drop_t *dt);

/**/
int main() {
	drop_t drops[SIZE_W/2];
	uint8_t col;
	
	/* Creamos gotas intercaladas por toda la anchura de la ventana */
	for(col = 0; col < SIZE_W/2; col++) drops[col] = createDrop(col*2);
	
	/* Configuracion de UART y limpieza inicial de pantalla */
	UART_Ini(0, 1000000, 8, 0, 1);
	UART_clrscr(0); /* Limpia y posiciona el cursor al inicio */
	UART_puts(0, "Presiona una tecla para iniciar...");
	UART_getchar(0);
	UART_clrscr(0);
	
    while (1) {
		/* Dibujamos las gotas en pantalla */
		for(col = 0; col < SIZE_W/2; col++) drawDrop(&drops[col]);
		
		delay(100);     /* Espera de 100ms para ver todos los caracteres */
		UART_clrscr(0); /* Limpieza de pantalla para actualizacion de valores */
		timer++;        /* Uso para determinar apariciones */
    }
}



/**/
drop_t createDrop(uint8_t posX) {
	drop_t dt;
	uint8_t k;
	dt.draw = 0;
	dt.index_current = 0;
	dt.global_pos_x = posX;
	dt.global_pos_y = myRand(SEED) % (SIZE_H - MAX_LONG);
	dt.sleep_timer =  (myRand(SEED) % 15) + timer;
	
	for(k = 0; k<MAX_LONG; k++) {
		dt.values[k] = caracteres[(myRand(SEED) % (SIZE_CARACTERS))];
	}
	
	return dt;
}


/**/
void drawDrop(drop_t *dt) {
	if(dt->draw) {
		temp_row = 0;
		
		/* Mostramos todos los caracteres con indice menor a la posicion actual */
		UART_setColor(0, GREEN);
		while(temp_row < dt->index_current) {
			UART_gotoxy(0, dt->global_pos_x, dt->global_pos_y + temp_row);
			UART_putchar(0, dt->values[temp_row]);
			temp_row++;
		}
		
		/* Impresion de caracter puntero */
		UART_setColor(0, YELLOW);
		UART_gotoxy(0, dt->global_pos_x, dt->global_pos_y + temp_row);
		UART_putchar(0, dt->values[dt->index_current]);
		
		/* Actualizacion de propiedades */
		if(dt->global_pos_y < SIZE_H) {
			dt->global_pos_y++;
			
			if(dt->global_pos_y + dt->index_current > SIZE_H) {
				if(dt->index_current) dt->index_current--;
			} else {
				if(dt->index_current+1 < MAX_LONG) dt->index_current++;	
			}
			
		} else *dt = createDrop(dt->global_pos_x);
		
	} else {
		if(timer >= dt->sleep_timer) {
			dt->draw = 1;
		}
	}
}