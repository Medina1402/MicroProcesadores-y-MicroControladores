DOSSEG
.model tiny
.code
  public _outportb
  public _inportb
  public _getchar
  public _putchar

  ; =====================================================================
  ; Procedimiento: OUTPORTB
  ; Descripcion: 
  ; Entrada: [bp + 4] = PORT, [bp + 6] = DATA
  ; Salida:
  ; =====================================================================
  _outportb proc
    push bp
    mov bp, sp

    mov dx, [bp + 4] ; PORT
    mov al, [bp + 6] ; DATA
    out dx, al

    pop bp
    ret
  _outportb endp

  ; =====================================================================
  ; Procedimiento: INPORTB
  ; Descripcion: 
  ; Entrada: [bp + 4] = PORT
  ; Salida: BYTE in AL
  ; =====================================================================
  _inportb proc
    push bp
    mov bp, sp

    mov dx, [bp + 4] ; PORT
    in al, dx

    pop bp
    ret
  _inportb endp

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

END