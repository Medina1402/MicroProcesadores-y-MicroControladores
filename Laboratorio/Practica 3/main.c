/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */
#include "stdio.c"

#define LINES_BUS_ADDR 11
#define LINES_BUS_DATA 8


/* =====================================================================*/
extern unsigned char peek(unsigned int segment, unsigned int offset);
extern void poke(unsigned int segment, unsigned int offset, unsigned char data);

unsigned char TestDataBus(unsigned int segment, unsigned int offset);
unsigned int TestAddrBus(unsigned int segment, unsigned int offset);

unsigned char err_sRAM(unsigned char statusDataBus, unsigned int statusAddrBus, unsigned char showStatus);
void __printTestDataBus(unsigned char status);
void __printTestAddrBus(unsigned int status);
/* =====================================================================*/

int main(void){
    
    while(1) {
        err_sRAM(TestDataBus(0, 0x2200), TestAddrBus(0, 0x2200), 1);
        getchar();
    }

    return 0;
}

/*
*
*/
unsigned char TestDataBus(unsigned int segment, unsigned int offset) {
    unsigned char pattern = 0;
    unsigned char value;

    for(value = 1; value; value <<= 1) {
        poke(segment, offset, value); /* Escribimos 1 */
        /* Comparamos el contenido con lo que escribimos */
        if( peek(segment, offset) != value ) pattern |= value;
    }

    return pattern;
}

/*
*
*/
unsigned int TestAddrBus(unsigned int segment, unsigned int offset) {
    unsigned char match_pattern = 0xAA;
    unsigned int ram_disp = 1;
    unsigned int pattern = 0;

    while(ram_disp < (1 << LINES_BUS_ADDR)) {
        poke(segment, offset + ram_disp, pattern);
        ram_disp <<= 1;
    }

    poke(segment, offset, match_pattern);

    for(ram_disp = 1; ram_disp < (1 << LINES_BUS_ADDR); ram_disp <<= 1) {
        if( peek(segment, offset + ram_disp) == match_pattern || peek(segment, offset + ram_disp) != 0) {
            pattern |= ram_disp;
        }
    }

    return pattern;
}

/* ================= PRINT TEST ================= */
/**
 * Busca los bits activos en el "status" e indica el estado (0 = ok, 1 = err)
 * @param status Bits a recorrer (numero hexadecimal <representacion binaria cruda>)
 */
void __printTestDataBus(unsigned char status) {
    unsigned char _count = 0;
    puts("*** TEST DATA BUS ***\n\r");

    while(_count<8) {
        puts(" > Line ");
        putchar(_count>9 ? _count+7+'0' :_count+'0');
        puts(": ");

        if(status & (1 << _count)) puts(" - ERR\n\r");
        else puts(" - OK\n\r");

        _count++;
    }
    puts("\n\r");
}

/**
 *
 * @param status
 * @param linesAddr
 */
void __printTestAddrBus(unsigned int status) {
    unsigned int _count = 0;
    puts("*** TEST ADDR BUS ***\n\r");

    while(_count<11) {
        puts(" > Line ");
        putchar(_count>9 ? _count+7+'0' :_count+'0');
        puts(": ");

        if(status & (1 << _count)) puts(" - ERR\n\r");
        else puts(" - OK\n\r");

        _count++;
    }
    puts("\n\r");
}
/**
 *
 * @param statusDataBus Estado del bus de datos (usar el retorno de testAddrBus)
 * @param statusAddrBus Estado del bus de direcciones (usar el retorno de testAddrBus)
 * @param showStatus Mostrar estado en caso de no ser un error catastrofico
 * @return
 */
unsigned char err_sRAM(unsigned char statusDataBus, unsigned int statusAddrBus, unsigned char showStatus) {
    if(showStatus) {
        __printTestAddrBus(statusAddrBus);
        __printTestDataBus(statusDataBus);
    }

    if(statusDataBus >= 0xFF && statusAddrBus >= 0xFF) {
        puts("\n\r=== \n\r||| *** FALLO CATASTROFICO *** ||| \n\r===\n\r");
        return 1;
    }
    return 0;
}