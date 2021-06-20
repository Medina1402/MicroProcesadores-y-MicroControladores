; =====================================================================
; @author Abraham Medina Carrillo
; @email abraham.medina.carrillo@uabc.edu.mx
;
; @website https://github.com/Medina1402
; @repository ** private **
; =====================================================================

; Corrimiento a la izquierda sin acarreo de un registro compuesto
; Ejemplo:
;    > Z = 0000 0000  1000 0000
;    > LSL_RegC R30, R31
;    > Z = 0000 0001  0000 0000
.MACRO LSL_RegC
	LSL @0 ; Desplazamiento a la izq sin tomar en cuenta el acarreo
	ROL @1 ; Desplasamiento a la izq tomando en cuenta el acarreo
.ENDM

; Corrimiento a la derecha sin acarreo de un registro compuesto
; Ejemplo:
;    > Z = 0000 0001  0000 0000
;    > LSL_RegC R30, R31
;    > Z = 0000 0000  1000 0000
.MACRO LSR_RegC
	LSR @1 ; Desplazamiento a la derecha sin tomar en cuenta el acarreo
	ROR @0 ; Desplasamiento a la derecha tomando en cuenta el acarreo
.ENDM

; -----------------------------------------------------------------------------------

LDI R30, 128
LDI R29, 1

start:
	LSR_RegC R28, R29
	LSL_RegC R30, R31
    rjmp start
