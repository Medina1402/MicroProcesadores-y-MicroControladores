/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#define BYTE unsigned char
#define WORD unsigned int
#define PORTw 0xFFFF
#define SEGMENT 0
#define INTR 8
#define OFFSET_COMPILE 0x100
#define PA 0x40
#define PB 0x41
#define PC 0x42
#define RCtr 0x43
#define PTOs_all_out 0x80

/* External functions io */
extern void putchar(char dato);
extern char getchar(void);
void puts(char *str) {while(*str) putchar(*str++);}
extern void pokew(unsigned int segment, unsigned int offset, unsigned int data);
extern void outportb(WORD port, BYTE data);
extern BYTE inportb(WORD port);
void SetBitPort(WORD Puerto, BYTE num_bit) {
    outportb(Puerto, inportb(Puerto) | (1<<num_bit));
}
void ClrBitPort(WORD Puerto, BYTE num_bit) {
    outportb(Puerto, inportb(Puerto) & ~(1<<num_bit));
}

 /*Bandera para indicar el Seg. */
volatile static unsigned char SecFlag;

/* Manejador de Interrupcion: ocurre 20 veces por segundo */
void interrupt ManejadorISR(void) {
    static unsigned char Cont = 0;
    Cont++; /* Conteo de Ticks */
    if(Cont == 20) {
        SecFlag = 1;
        Cont = 0;
    }
} 

/**
 * Si pasaron las N interrupciones SecFlag estara en 1
 * entonces se regresara a 0 y retornara 1
*/
unsigned char TimerSecFlag(void) {
    if(SecFlag == 1 ) {
        SecFlag = 0;
        return 1;
    }

    return 0;
} 

/**/
void Timer_Ini( void ){
    SecFlag = 0;
    /* Inicializarvector --faltapokeW(WORD segment, WORD offset, WORD value) -- */
    pokeW(SEGMENT, INTR * 4, OFFSET_COMPILE + (unsigned int) ManejadorISR); /* Determinar offset */
    pokeW(SEGMENT, (INTR * 4) + 2, _CS);
}

/**/
void SetClock(BYTE hours, BYTE minutes, BYTE seconds);
void UpdateClock();
void DisplayClock();
void PrintDec2d(BYTE);
void Delay(BYTE);

/*Variables globales*/
BYTE seg = 0, min = 0, hrs = 0; 

int main() {
    Timer_Ini();
    SetClock(23, 59, 49);

    outportb(RCtr, PTOs_all_out);
    while(1) {
        if(TimerSecFlag()) {
            UpdateClock();
            DisplayClock();

            outportb(PB, seg);
        }
        Delay(100);
    }

    return 0;
}


/**/
void UpdateClock() {
    if(++seg >= 60) {
        seg = 0;
        if(++min >= 60) {
            min = 0;
            hrs = (++hrs) % 24;
        }
    }
}

/* Formato de Reloj HH:MM:SS */
void DisplayClock() {
    PrintDec2d(hrs); putchar(':');
    PrintDec2d(min); putchar(':');
    PrintDec2d(seg); puts("\r");
}

/* Pausa */
void Delay(BYTE time) {
    while(time--);
}

/*Impresio de # decimales de 2 cifras*/
void PrintDec2d(BYTE dato) {
    putchar(dato/10 + '0');
    putchar(dato%10 + '0');
}

/* Modifica el Reloj */
void SetClock(BYTE hours, BYTE minutes, BYTE seconds) {
    hrs = hours;
    min = minutes;
    seg = seconds;
}