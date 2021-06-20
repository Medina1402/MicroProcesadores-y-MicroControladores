; =====================================================================
; @author Abraham Medina Carrillo
; @email abraham.medina.carrillo@uabc.edu.mx
;
; @website https://github.com/Medina1402
; @repository ** private **
; =====================================================================

.global Delay104uS
.global TXoPin

.set PORTB, 0x05

; ----------------------------------------------------------
; Retardo de 104uS para uC ATMega1280/2560 a 16MHz, BAUD de 9600
; 1 ciclo = 62.5nS
; 1664 ciclos = 104uS
; ----------------------------------------------------------
Delay104uS:
	push r17
	push r16
   
	ldi r17, 16
	ciclo:
	nop
	nop
	nop
	ldi r16, 24
	ciclo2:
	
	dec r16
	cpi r16, 0
	brne ciclo2

	dec r17
	cpi r17, 0
	brne ciclo

	pop r16
	pop r17
	ret

; ----------------------------------------------------------
; Transmisión de dato dato de 8 bits vía comunicación serie
; ----------------------------------------------------------
; Condiciones: 9600, 8, E, 1
; -> Velocidad 9600 (BPS, delay de 104uS)
; -> Datos de 8 bits
; -> Paridad par
; -> 1 Bit de paro
; ----------------------------------------------------------
TXoPin:
	push R23
	push R22
	push R21
	push R20

	; Cantidad de bits de datos a transmitir
	ldi R22, 0x08
	ldi R21, 1 ; Uso en transmission_8bits CPSE
	ldi R20, 0

	; ---------------------------------
	;         Start Bit = 0
	; ---------------------------------
	cbi PORTB, 7
	call Delay104uS

	; ---------------------------------
	;    Transimision de los 8 bits
	; ---------------------------------
	transmission_8bits:
	ldi R23, 0x01 ; Mascara par bit 0
	and R23, R24  ; Filtramos solo el bit 0

	; CPSE son dos ciclos cuando no es igual
	cpse R23, R21
	rjmp next
	inc R20
	sbi PORTB, 7

	next:
	cpi R23, 0
	brne continue
	nop
	cbi PORTB, 7

	continue:
	call Delay104uS

	ror R24
	dec R22
	brne transmission_8bits

	; ---------------------------------
	;         Bit de paridad
	; ---------------------------------
	ldi R23, 0x01  ; Mascara para paridad (bit menos significativo activado = paridad par)
	and R23, R20
	ldi R22, 1

	cpse R23, R22
	rjmp next2
	sbi PORTB, 7

	next2:
	cpi R23, 0
	brne stop_bit
	cbi PORTB, 7

	call Delay104uS

	; ---------------------------------
	;     Bit de paro en el PB7
	; ---------------------------------
	stop_bit:
	sbi PORTB, 7
	call Delay104uS

	pop R20
	pop R21
	pop R22
	pop R23
	ret