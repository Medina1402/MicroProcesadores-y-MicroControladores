; ================================================================
; @author Abraham Medina Carrillo
; @email abraham.medina.carrillo@uabc.edu.mx
;
; @website https://github.com/Medina1402
; @repository ** private **
; ================================================================

.include "m1280def.inc"

; -----------------------------------------------------------------
; En las ultimas direcciones de la RAM (antes de llegar a la extension 0x2200), evitando
; sobreescribir la pila, se utilizara la direccion 0x20FF como mayor direccion, las cuales
; seran utilizadas como registros para la generacion de numeros psudo-aleatorios
; utilizano el algoritmo Congruencial Multiplicativo
.equ RAND_X0 = 0x20FF ; Semilla inicial, debe ser impar
.equ RAND_A  = 0x20FE ; Valor de A=3+8K o 5+8K donde K=0,1,2,3...
.equ RAND_B  = 0x20FD ; Incremento, por lo general se omite al ser 0
.equ RAND_M  = 0x20FC ; Modulo, M=2^g donde g = 0,1,2,3....
; Periodo de vida o N son la cantidad de iteraciones que se realizara la operacion
; la cual se aconseja sea N = m/4 = 2^(g-2)
; ================================================================
.MACRO NRAND_8_INIT
	PUSH R16

	LDI R16, @0
	STS RAND_X0, R16

	LDI R16, @1
	STS RAND_A, R16

	LDI R16, @2
	STS RAND_B, R16

	POP R16
.ENDM

; ================================================================
; Genera un numero pseudo-aleatorio de 8 bits utilizando el algoritmo
; algoritmo Congruencial Multiplicativo
; X0 + 1 = MOD(A * X0 + B, M)
; Retorna en Rd --> 3 < d <= 31
; ================================================================
.MACRO NRAND_8
	PUSH R0 ; RAND_X0
	PUSH R1 ; RAND_A
	PUSH R2 ; RAND_B

	LDS R0, RAND_X0
	LDS R1, RAND_A
	LDS R2, RAND_B

	MUL R0, R1
	ADD R0, R2
	MOV @0, R0

	STS RAND_X0, R0

	POP R2
	POP R1
	POP R0
.ENDM

; ================================================================
; Genera un numero random para cada Rd de manera independiente
; juntos formaran el registro de 16 bits
; ================================================================
.MACRO NRAND_16
	NRAND_8 @0 ; Parte alta
	NOP ; Peque�a pausa para evitar errores de escritura
	NRAND_8 @1 ; Parte Baja
	ADD @1, @2 ; Para una mayor cantidad de numeros psudo-aleatorios
.ENDM


START:
NRAND_8_INIT 37, 13, 3 ; Valores iniciales para generar #pseudo-aleatorios
; ------------------------------
; CICLO DE 1,000,000 ITERACIONES
; > R16, R17, R18 son acumuladores para las iteraciones
; cada uno con valor de 100 o 0x64 (100x100x100) = 1000000
; > R19, R20 sera la Rx_16Bits donde R19 es la parte alta
; y R20 la parte baja (Rx_16Bits = R19:R20).
;
LDI R16, 100
LDI R21, 0x00 ; Contador general (INDICE para los numeros aleatorios)
START_C0:
	CPI R16, 0x0
	BRNE CONTINUE_C0
	RJMP END_C0
	CONTINUE_C0:
	LDI R17, 100
	START_C1:
		CPI R17, 0
		BRNE CONTINUE_C1
		RJMP END_C1
		CONTINUE_C1:
		LDI R18, 100
		START_C2:
			CPI R18, 0x0
			BRNE CONTINUE_C2
			RJMP END_C2
			CONTINUE_C2:
			; ------------------------------------------------------
			; Por I/O solo podemos utilizar 8 bits (1 byte)
			; por lo que se haran dos escrituras, la parte alta R19
			; y la parte baja R20, con lo que formaremos una palabra
			; (WORD o 2 bytes) con numeros psuedo-aleatorios
			INC R21

			NRAND_16 R19, R20, R21
			OUT PORTA, R19
			OUT PORTA, R20
			; ------------------------------------------------------

			DEC R18
			RJMP START_C2
		END_C2:
		DEC R17
		RJMP START_C1 
	END_C1:
	DEC R16
	RJMP START_C0
END_C0:
; ------------------------------
RJMP START
