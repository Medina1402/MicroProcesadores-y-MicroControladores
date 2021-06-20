package main

/*
* Creado por: Abraham Medina Carrillo
* Compilar: go build configUART.go
* Correr sin compilar: go run configUART.go <fosc> <com> <baud> <nbits> <paridad> <stop> <u2x>
* Ejecutar: configUART <fosc> <com> <baud> <nbits> <paridad> <stop> <u2x>
*
*/

import (
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
	"strings"
)

func atoiArgs(value string) int {
	num, err := strconv.Atoi(value)
	if err != nil {
		fmt.Printf("Error al leer " + value)
	}
	return num
}

func generateFile(args []string) {
	counterErr := 0

	fosc    := atoiArgs(args[1])
	com     := atoiArgs(args[2])
	baud    := atoiArgs(args[3])
	nbits   := atoiArgs(args[4])
	parity  := atoiArgs(args[5])
	stop    := atoiArgs(args[6])
	u2x     := atoiArgs(args[7])

	ubrr := 0
	temp := 16 /*u2xn = 0*/
	if u2x == 1 { temp = 8 /*u2xn = 1*/ }
	ubrr = (fosc / (temp * (baud))) - 1

	if fosc < 1 { fmt.Println("* Frecuencia invalida"); counterErr++ }
	if com < 0 || com > 3 { fmt.Println("* COM = 0 | 1 | 2 | 3"); counterErr++ }
	if baud < 0 { fmt.Println("* BAUD invalido"); counterErr++ }
	if nbits < 5 || nbits > 9 { fmt.Println("* NBITS = 5 | 6 | 7 | 8 | 9"); counterErr++ }
	if parity < 0 || parity > 2 { fmt.Println("* PARIDAD = 0 | 1 | 2"); counterErr++ }
	if stop < 1 || stop > 2 { fmt.Println("* STOP = 1 | 2"); counterErr++ }
	if u2x < 0 || u2x > 1 { fmt.Println("* u2x = 0 | 1"); counterErr++ }

	if counterErr == 0 {
		UCSRxBn := 0x18 /* RXENn TXENn */
		UCSRxCn := 0 /* Todos en 0 -> UMSELx0/1 = 0 *Modo asincrono* */

		/*UBRR*/
		UBRRxH := fmt.Sprintf("\tUBRR%dH = 0x%X;\n", com, (ubrr >> 8) & 0xFF)
		UBRRxL := fmt.Sprintf("\tUBRR%dL = 0x%X;\n\n", com, ubrr & 0xFF)

		/*NBITS*/
		if nbits == 9 {
			UCSRxCn = 3 << 1 /* UCSZx1 UCSZx0 */
			UCSRxBn |= 1 << 2 /* UCSZx2 */
		} else {
			UCSRxCn = ((nbits - 5) & 3) << 1 /* UCSZx1 UCSZx0 */
		}

		/* Paridad */
		UCSRxCn |= ((parity * (parity+1)) & 6) << 3 /* UPMx1 UPMx0 */

		/* Bit de paro */
		UCSRxCn |= (stop & 2) << 2 /* USBSx */

		/**/
		UCSRxB := fmt.Sprintf("\tUCSR%dB = 0x%X;\n", com, UCSRxBn)
		UCSRxC := fmt.Sprintf("\tUCSR%dC = 0x%X;\n", com, UCSRxCn)

		code := []byte("/*\n* UART Init Config\n" +
			"* > F_osc = " + strconv.Itoa(fosc) + "\n" +
			"* > Baud  = " + strconv.Itoa(baud) + "\n" +
			"*/\nvoid UART"+ strconv.Itoa(com) +"_Init() {\n" +
			"\t/* UBRR"+ strconv.Itoa(com) +" = "+ strconv.Itoa(ubrr) +" */\n" + UBRRxH + UBRRxL +
			"\t/* UCSR"+ strconv.Itoa(com) +"X: USART Control and Status Register "+ strconv.Itoa(com) +" X */\n" +
			"" + UCSRxB + UCSRxC + "}")

		err := ioutil.WriteFile("UART" + strconv.Itoa(com) + "_Init.c", code, 0777)
		if err != nil {
			fmt.Printf("*** Error al crear el archivo: %v ***", err)
		} else {
			fmt.Printf("*** Archivo creado ***")
		}
	} else {
		fmt.Println("** configUART <fosc> <com> <baud> <nbits> <paridad> <stop> <u2x> **")
	}
}

func main() {
	if len(os.Args) < 8 {
		fmt.Println("\n** configUART <fosc> <com> <baud> <nbits> <paridad> <stop> <u2x> **")

		common := strings.ToLower(os.Args[1])
		if common == ":help" || common == ":?" {
			fmt.Println("" +
				"\t> fosc: Frecuencia de oscilacion que opera el microcontrolador (Hertz).\n" +
				"\t> com: Numero de puerto a utilizar (de 0 a 3).\n" +
				"\t> baud: Velocidad de operacion (Bauds).\n" +
				"\t> nbits: Tamano en bits de los datos a transmitir (5 a 9).\n" +
				"\t> paridad: Paridad a operar ( 0: no paridad, 1: impar y 2:par ).\n" +
				"\t> stop: Cantidad de bits de paro (1 o 2 )\n" +
				"\t> u2x: habilitación de doble velocidad (1 o 0).")
		}
	} else {
		generateFile(os.Args)
	}
}
