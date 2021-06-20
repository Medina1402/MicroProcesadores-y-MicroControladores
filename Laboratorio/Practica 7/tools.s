/*
* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* =====================================================================
*/

.global myRand
.global delay

/*
* La semilla solo se toma como valor inicial, si dicho valor no cambia, no se tomara
* en cuenta para los siguientes calculos, si la semilla cambia se reiniciara tomando
* el nuevo valor de la semilla
* (*0x20FE) = Semilla actual <valor inicial/original de la semilla>
* (*0x20FF) = Xn <valor para la nueva evaluacion {numero pseudo-aleatorio}>
*/
myRand:
	push R16
	push R0

	; Si el valor semilla es igual, saltaremos a calcular el nuevo valor
	lds R16, 0x20FE
	cp R16, R24
	breq equal_seed

	; Nueva semilla
	sts 0x20FE, R24
	sts 0x20FF, R24

	; X0 + 1 = ((X0 * A) + B) % 0xFF
	; A = 13  |  B = 3
	equal_seed:
	lds R24, 0x20FF
	ldi R16, 13
	mul R24, R16
	mov R24, R0
	ldi R16, 3
	add R24, R16

	sts 0x20FF, R24

	pop R0
	pop R16
	ret

/*
* Se recibe por R24 la cantidad de veces que se repetira el ciclo
* de milisegundos, el valor minimo es 1, si se asigna un 0, por defecto
* se correra una vez (1ms)
*/
delay:
	push R16
	push R17

	ciclo:
	ldi R17, 44
	
	ciclo_1:
	ldi R16, 90
	
	ciclo_2:
	dec R16
	cpi R16, 0
	brne ciclo_2
	
	dec R17
	cpi R17, 0
	brne ciclo_1
	
	dec R24
	cpi R24, 0
	brne ciclo
	
	pop R17
	pop R16
	ret