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
  public _peek
  public _poke
  public _getchar
  public _putchar

  ; =====================================================================
  ; Procedimiento: PEEK
  ; Descripcion: Extrae el byte almasenado en el desplazamiento y segmento
  ;              que se especifica en los parametros
  ; Entrada: SEGMENTO = [SP|BP + 4], OFFSET = [SP|BP + 6]
  ; Salida: AL
  ; =====================================================================
  _peek proc
    push bp
    mov bp, sp

    push ds
    push bx

    xor ax, ax
    mov ds, [bp + 4] ; SEGMENTO
    mov bx, [bp + 6] ; OFFSET
    mov al, [bx]

    pop bx
    pop ds
    pop bp
    ret
  _peek endp

  ; =====================================================================
  ; Procedimiento: POKE
  ; Descripcion: Almacena el byte en el segmento y desplazamiento especificado
  ;              en los parametros
  ; Entrada: SEGMENTO = [SP|BP + 4], OFFSET = [SP|BP + 6], DATA = [SP|BP + 8]
  ; Salida: -
  ; =====================================================================
  _poke proc
    push bp
    mov bp, sp

    push ds
    push dx
    push bx

    xor dx, dx
    mov ds, [bp + 4] ; SEGMENTO
    mov bx, [bp + 6] ; OFFSET
    mov dl, [bp + 8] ; DATA
    mov [bx], dl

    pop bx
    pop dx
    pop ds
    pop bp
    ret
  _poke endp

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

    push ax
    push dx
    
    mov dl, [bp + 4]
    mov ah, 2
    int 21h
    
    pop dx
    pop ax
    pop bp
    ret
  _putchar ENDP

end