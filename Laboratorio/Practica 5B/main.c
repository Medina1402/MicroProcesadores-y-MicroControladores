/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#define BYTE unsigned char
#define WORD unsigned int
#define PA 0x40
#define PB 0x41
#define PC 0x42
#define RCtr 0x43
#define PTOs_all_out 0x80

/* External functions io */
extern void putchar(char dato);
extern char getchar(void);
void puts(char *str) {while(*str) putchar(*str++);}

/* External functions inport / outport */
extern void outportb(WORD port, BYTE data);
extern BYTE inportb(WORD port);
/*void outportb(WORD port, BYTE data) {
    asm mov dx, port
    asm mov al, data
    asm out dx, al 
}*/

/*BYTE inportb(WORD port) {
    asm mov dx, port
    asm in al, dx
}*/

/* Prototypes port bits */
BYTE ReverseBYTE(BYTE dato);
void SetBitPort(WORD Puerto, BYTE num_bit);
void ClrBitPort(WORD Puerto, BYTE num_bit);
void NotBitPort(WORD Puerto, BYTE num_bit);
BYTE TstBitPort(WORD Puerto, BYTE num_bit);
void ReverseBitsPort(WORD Puerto);

/*Prototypes*/
void SLEEP(BYTE timer) { while(timer--); }
void printBin(BYTE dato);
void ShowInPort(BYTE data);
void ViewLED(WORD PortVCC, BYTE BitVCC, WORD PortGND, BYTE BitGND);

int main(void) {
    char counter = 0, dato;

    puts("Practica 5b\n\r");
    
    /* 10001000 = 88h = PC7-PC4 de entrada */
    outportb(RCtr, 0x88);

    for(dato = 0; counter < 8; counter++) {
        getchar(); /* Al presionar se captura el puerto */
        dato |= TstBitPort(PC, 4);
        printBin(dato);
        puts("\n\r");
        if(counter < 7) dato <<= 1;
    }

    puts("\n\r---\n\r>>> Posicion original: ");
    printBin(dato);

    puts("\n\r>>> Posicion invertida: ");
    dato = ReverseBYTE(dato);
    printBin(dato);

    puts("\n\r\n\rPresiona para continuar...");
    getchar();
    puts("\n\r\n\r ** Ejecutando programa sobre hardware **");

    /*Todos los puertos de salida*/
    outportb(RCtr, PTOs_all_out);
    ShowInPort(dato);

    return 0;
}

/* Invierte un BYTE */
BYTE ReverseBYTE(BYTE data) {
    BYTE counter, invert = 0;

    for(counter = 0; (0x80)>>counter; counter++) {
        if(counter<4) invert |= (data & (0x80) >> counter) >> (7 - (counter*2));
        else invert |= (data & (0x80) >> counter) << ((counter*2) - 7);
    }

    return invert;
}

/**/
void ViewLED(WORD PortVCC, BYTE BitVCC, WORD PortGND, BYTE BitGND) {
    SetBitPort(PortVCC, BitVCC);
    ClrBitPort(PortGND, BitGND);
}

/* Creada para mostrar de forma correcta el arreglo de LEDS
* >> Debido a que al poner un 0 en la salida simula tierra
* >> por lo que no es necesario cambiar el Registro de control
*/
void ShowInPort(BYTE data) {
    BYTE counter = 0, aux;

    while(1) {
        /* Solo escribiremos los 1 <filtro>*/
        aux = data & (1 << counter);

        if(aux) {            
            if(counter > 5) { /*Bit 6 y 7*/
                outportb(RCtr, 0x92);
                if(counter == 7) ViewLED(PC, 3, PC, 4);
                else ViewLED(PC, 4, PC, 3);
            } else if(counter > 3) { /*Bit 4 y 5*/
                outportb(RCtr, 0x8A);
                if(counter == 5) ViewLED(PC, 3, PA, 1);
                else ViewLED(PA, 1, PC, 3);
            } else if(counter > 1) { /*Bit 2 y 3*/
                outportb(RCtr, 0x98);
                if(counter == 3) ViewLED(PB, 2, PC, 3);
                else ViewLED(PC, 3, PB, 2);
            } else { /*Bit 0 y 1*/
                outportb(RCtr, 0x89);
                if(counter == 1) ViewLED(PA, 1, PB, 2);
                else ViewLED(PB, 2, PA, 1);
            }

            SLEEP(20);
        }

        /*Reset counter*/
        if(counter++ >= 7) counter = 0;
    }
}

/**/
void SetBitPort(WORD Puerto, BYTE num_bit) {
    outportb(Puerto, inportb(Puerto) | (1<<num_bit));
}

/**/
void ClrBitPort(WORD Puerto, BYTE num_bit) {
    outportb(Puerto, inportb(Puerto) & ~(1<<num_bit));
}

/**/
void NotBitPort(WORD Puerto, BYTE num_bit) {
    outportb(Puerto, inportb(Puerto) ^ (1<<num_bit));
}

/**/
BYTE TstBitPort(WORD Puerto, BYTE num_bit) {
    return (inportb(Puerto) & (1<<num_bit)) != 0;
}

/**/
void ReverseBitsPort(WORD Puerto) {
    outport(Puerto, ReverseBYTE(inportb(Puerto)));
}

/**/
void printBin(BYTE dato) {
    BYTE msk = 0x80;
    do {
        putchar( (dato & msk) ?'1' :'0');
        msk >>= 1;
    } while(msk);
}