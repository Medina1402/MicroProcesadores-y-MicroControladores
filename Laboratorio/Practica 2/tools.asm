; =====================================================================
; @author Abraham Medina Carrillo
; @email abraham.medina.carrillo@uabc.edu.mx
;
; @website https://github.com/Medina1402
; @repository ** private **
; =====================================================================

DOSSEG
.model tiny
.code
	public _getchar
	public _putchar

  ; =====================================================================
  ; Procedimiento: GETCHAR
  ; Descripcion: Procedimiento que lee un caracter del teclado haciendo
  ;              uso de la int 21h servicio 08h del S.O. ** SIN ECHO **
  ; Entrada: -
  ; Salida: AL = Caracter tecleado
  ; =====================================================================
  _getchar proc
    mov ah, 08
    int 21h
    ret
  _getchar endp

  ; =====================================================================
  ; Procedimiento: PUTCHAR
  ; Descripcion: Procedimiento que escribir un caracter del teclado haciendo
  ;              uso de la int 21h servicio 02h del S.O.
  ; Entrada: BP + 4 = Caracter a imprimir
  ; Salida: -
  ; =====================================================================
  _putchar PROC
    push bp
    mov bp, sp
    
    mov dl, [bp + 4]
    mov ah, 2
    int 21h
    
    pop bp
    ret
  _putchar ENDP

end