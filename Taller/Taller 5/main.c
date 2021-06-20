/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#define WORD unsigned int
#define BYTE unsigned char

#define PA 0x40
#define PB 0x41
#define PC 0x42
#define RCtrl 0x43
#define WCtrl 0x81 /*1000 0001*/

#define INTR 8 /* Pos en el vector de interrupcion */
#define TISR 4 /* Limite de Tics ocacionados por la interrupcion */

/*--- IO ---*/
void pokeW(WORD segment, WORD offset, WORD data);
void outportb(WORD port, BYTE data);
BYTE inportb(WORD port);

/*--- Actions ---*/
void DisplayDato(BYTE number);
BYTE MaxMin(BYTE max, BYTE min, BYTE value);
volatile static BYTE btn_SW1, btn_SW2, SW1, SW2;

/*--- Interruptions ---*/
volatile static BYTE refresh_f;
BYTE TimerRefreshFlag();
void interrupt ManejadorISR();
void Timer_Ini();


/*---*/
int main() {
	BYTE PC_value, current_counter = 0;

	Timer_Ini();
	outportb(RCtrl, WCtrl); /*PA, PB y PC<7-4> Salida, PC<3-0> Entrada*/

	while(1) {
		if(TimerRefreshFlag()) DisplayDato(current_counter);

		PC_value = ~inportb(PC); /* Guardamos los bits invertidos del puerto */
		btn_SW1 = (PC_value & 0x01) != 0;
		btn_SW2 = (PC_value & 0x02) != 0;

		/* INCREMENTO O DECREMENTO */
		if(SW1 && !btn_SW1) {
			if(SW2 && !btn_SW2) SW2 = 0;
			else current_counter = MaxMin(9, 0, current_counter + 1);
			SW1 = 0;
		} else if(SW2 && !btn_SW2) {
			SW2 = 0;
			current_counter = MaxMin(9, 0, current_counter - 1);
		}
	}

	return 0;
}


/*-----------------------------------------*/
/*--- IO ---*/

/*Escribe un BYTE en el puerto indicado*/
void outportb(WORD port, BYTE data) {
	asm mov dx, port
	asm mov al, data
	asm out dx, al
}

/*Extrae un BYTE en el puerto indicado*/
BYTE inportb(WORD port) {
	asm mov dx, port
	asm in al, dx
}

/*
* Escribe una palabra el la localidad determinada por el
* segmento y el offset <mov [ds + bx], ax>
*/
void pokeW(WORD segment, WORD offset, WORD data) {
	asm push ds
	asm mov ds, segment
	asm mov bx, offset
	asm mov ax, data
	asm mov [bx], ax
	asm pop ds
}


/*-----------------------------------------*/
/*--- Interruptions ---*/

/*
* Esta interrupcion se escribe en la llamada de la INT8
* cada que se llame a la INT8, esta llamara a ManejadorISR
* tipicamente 18.xx veces por seg, en el interperte son 20
*/
void interrupt ManejadorISR() {
	static BYTE count = 0, swt_1 = 0, swt_2 = 0;

	if((++count) == TISR) {
		refresh_f = 1;
		count = 0;
	}

	/* Si esta presionado por lo menos 0.1seg se habilita SW1 */
	if(btn_SW1) {
		if((++swt_1) == 2) SW1 = 1;
	} else swt_1 = 0;

	/* Si esta presionado por lo menos 0.1seg se habilita SW2 */
	if(btn_SW2) {
		if( (++swt_2) == 2 ) SW2 = 1;
	} else swt_2 = 0;
}

/* Si la bandera esta activa, retorna 1 y asigna 0 de nuevo */
BYTE TimerRefreshFlag() {
	return (refresh_f) ?!(--refresh_f) :0;
}

/*
* Escribimos en memoria el remplazo de la INT8 determinando 
* su localidad (ManejadorISR) y la localidad actual (CS) para cuando 
* finalice la nueva interrupcion, retorne al offset correspondiente 
*/
void Timer_Ini() {
	refresh_f = 0;
	SW1 = 0;
	SW2 = 0;

	pokeW(0, INTR * 4, (WORD) ManejadorISR);
	pokeW(0, (INTR * 4) + 2, _CS);
}


/*-----------------------------------------*/
/*--- Actions ---*/

/*
* Selecciona los bits a encender en el Display
* >> ABCD EFGH = #Display
*/
void DisplayDato(BYTE number) {
	static BYTE Display7Seg[10] = {
		0xFC, /* 1111 1100 =  0 */
		0x60, /* 0110 0000 =  1 */
		0xDA, /* 1101 1010 =  2 */
		0xF2, /* 1111 0010 =  3 */
		0x66, /* 0110 0110 =  4 */
		0xB6, /* 1011 0110 =  5 */
		0xBE, /* 1011 1110 =  6 */
		0xE0, /* 1110 0000 =  7 */
		0xFE, /* 1111 1110 =  8 */
		0xE6, /* 1110 0110 =  9 */
	};
	outportb(PA, Display7Seg[number]);
}

/*
* Verifica si el BYTE se encuentra en el rango permitido
* en caso contrario retorna el maximo o minimo segun sea el caso
*/
BYTE MaxMin(BYTE max, BYTE min, BYTE value) {
	return (value > min) ?((value < max) ?value :max) :min;
}
