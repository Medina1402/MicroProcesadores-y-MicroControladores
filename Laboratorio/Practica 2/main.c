/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#define MAX_SIZE 15

extern void putchar(char data);
extern char getchar(void);

void gets(char *str);
void puts(char *str);
void itoa(unsigned int number, char* str, unsigned char base);
unsigned int atoi(char *str);
void println() { puts("\n\r"); }

char msg[] = "Ingresa el numero a convertir: ";
char dec[] = "   Dec: ";
char hex[] = "   Hex: ";
char bin[] = "   Bin: ";
char numero[MAX_SIZE];
char conversion[MAX_SIZE];
unsigned int decimal = 65535;

#ifndef TEST_HARNESS
int main(void) {
	while(1) {
		puts(msg); gets(numero);
		decimal = atoi(numero);

		/* Base 10 */
		itoa(decimal, conversion, 10);
		puts(dec); puts(conversion);
		println();

		/* Base 16 */
		itoa(decimal, conversion, 16);
		puts(hex); puts(conversion);
		println();

		/* Base 2 */
		itoa(decimal, conversion, 2);
		puts(bin); puts(conversion);
		println();

		/* End Line */
		println();
	}

	return 0;
}
#endif

/*
* Impresion de una cadena de caracteres
*/
void puts(char *str) {
	while(*str) putchar(*str++);
}

/*
* Captura de una cadena de caracteres
*/
void gets(char *str) {
	const unsigned int MAX_SIZE_STR = 15;
	unsigned int counter = 0;
	static char key;

	key = getchar();

	while(key != 13) { /* !Enter */
		if(key != 8) { /* !Backspace */
			
			if(counter < MAX_SIZE_STR - 1 && str) {
				counter++;
				*str++ = key;
				putchar(key);
			}

		} else if(counter > 0) {
			putchar(8); putchar(32); putchar(8);
			counter--;
			*str--;
		}

		key = getchar();
	}
	*str = 0;
	putchar(10);
}

/*
* Conversion de un numero base 10 a la designada
* Numero maximo a convertir 65535
* 65535 + 1 = 0, debido a que pasa un overflow
*/
void itoa(unsigned int number, char *str, unsigned char base) {
	/*1-9:48-57 | A-Z:65-90*/
	char num_temp[15];
	unsigned count = 0;
	
	if(number < 1 || base < 2) {
		*str++ = '0';
		*str = 0;
		return;
	}

	while(number) {
		if((number % base) > 9) num_temp[count++] = (number % base) + '0' + 7;
		else num_temp[count++] = (number % base) + '0';
		number -= (number % base);
		number /= base;
	}

	/*invertir orden*/
	while(count > 0) {
		*str++ = num_temp[--count];
	}

	*str = 0;
}

/*
* Conversion de una cadena a su equivalente numerico
* La asignacion terminara al encontrar un caracter no numerico
* -> "12.25" = 12
* -> "58a58" = 58
* -> "-25" = 0
* -> "" = 0
* Numero maximo a convertir 65535
* -> 65535 + 1 = 0, debido a que pasa un overflow
*/
unsigned int atoi(char *str) {
	unsigned int number = 0;

	while(*str && *str >= '0' && *str <= '9') {
		number += *str - '0';
		*str++;
		if(*str && *str >= '0' && *str <= '9') number *= 10;
	}

	return number;
}