; =====================================================================
; @author Abraham Medina Carrillo
; @email abraham.medina.carrillo@uabc.edu.mx
;
; @website https://github.com/Medina1402
; @repository ** private **
; =====================================================================

.model tiny
  locals
.data
  msg1 DB 'Cadena original: ', 0
  msg2 DB 'Cadena modifi..: ', 0

  DIMENSION EQU 5 ; Valor constante
  ORIGINAL DB 'MICROPROCESADORESYMICROCONTROLADORES', 0
  MODIFICADA DB ?

.code
  org 100h

PRINCIPAL proc
  mov sp, 0fffh

  ; Loop principal para evitar errores (se debe matar el proceso)
  @main:
    call str_enter
    mov dx, offset msg1
    call puts
    mov dx, offset ORIGINAL
    call puts
    call str_enter

    ; ZigZag
    mov bx, dx
    mov dx, offset MODIFICADA ; uso si la cantidad de caracteres = 1
    mov al, DIMENSION
    call ZigZag

    ; Cadena modificada
    mov dx, offset msg2
    call puts
    mov dx, offset MODIFICADA
    call puts

    call str_enter
    call getchar
  jmp @main

  ; =====================================================================
  ; Procedimiento: ZIGZAG
  ; Descripcion: Procedimiento que imprime una cadena en formato de ZigZag
  ;              escribiendo por columnas, e imprime la captura resultante
  ;              al leer en orden de renglones, la cadena destino debe estar,
  ;              inmediata despues de terminar la cadena origen.
  ; Entrada: -
  ; Salida: -
  ; =====================================================================
  ZigZag proc
    ; Cantidad de renglones = 1
    cmp al, 1
    jne no_is_one
    
    ; Copia la cadena en bx a la direccion en memoria dada por dx
    call str_copy
    jmp return_zigzag

    no_is_one:
      mov cx, bx
      call str_len ; CL = Longitud de la cadena original
      mov ch, 1 ; CH = Contador actual (posicion recorrida)
      xor dx, dx ; DL = Contador I, DH = Contador J
      mov ah, al ; Cantidad de caracteres (K)

      ; ####################################################
      ;                  Primer renglon
      ; ####################################################
      p_first_r:
        call fl_row
        cmp dl, cl
        jb p_first_r
      call str_enter


      ; ####################################################
      cmp ah, 2
      je p_internal_row_1_close

      is_not_two:
      mov dh, 1
      p_internal_row_1:
        mov al, 1
        mov dl, dh

        p_interal_row_2:
          cmp al, 1
          je orientation_is_true

          orientation_is_false: ; i = (k-1)*2 - (k-j-1)*2
            push ax
            call copy_rel_str

            push cx
            xor cx, cx
            mov cl, ah ; k
            sub cl, dh ; k - j
            dec cl ; k - j - 1
            add cl, cl ; (k-j-1) + (k-j-1)
            mov ch, ah ; k
            dec ch ; k - 1
            add ch, ch ; (k-1) + (k-1)
            sub ch, cl ; [(k-1) + (k-1)] - [(k-j-1) + (k-j-1)]
            add dl, ch ; i += (k-1)*2 - (k-j-1)*2
            ; impresiones
            mov cl, ch
            mov ch, 0
            call str_blank
            pop cx
            jmp p_interal_row_2_continue

          orientation_is_true: ; i = (k-j-1)*2
            push ax
            call copy_rel_str

            push cx
            xor cx, cx
            mov cl, ah ; k
            sub cl, dh ; k - j
            dec cl ; k - j - 1
            add cl, cl ; (k-j-1) + (k-j-1)
            add dl, cl ; i += (k-j-1) + (k-j-1)
            ; impresiones
            mov ch, 0
            call str_blank
            pop cx

          p_interal_row_2_continue:
            pop ax
            call invert_al
            cmp dl, cl
            jb p_interal_row_2

        ; p_interal_row_1_continue:
          call str_enter
          inc dh
          dec ah
          cmp dh, ah
          jb p_dh_low_ah

          p_dh_high_ah:
            inc ah
            jmp p_internal_row_1_close
          p_dh_low_ah:
            inc ah
            jmp p_internal_row_1
      p_internal_row_1_close:

      ; ####################################################
      ;                  Ultimo renglon
      ; ####################################################
      mov dl, ah
      dec dl
      p_last_r:
        call fl_row
        cmp dl, cl
        jb p_last_r
      call str_enter

      ; Agrega el fin de cadena
      push bx
      call inc_bx_cx
      mov byte ptr [bx], 0
      pop bx

    return_zigzag: ret
  ZigZag endp

  ; #####################################################################
  ; #####################################################################
  ;                           HERRAMIENTAS
  ; #####################################################################
  ; #####################################################################

  ; =====================================================================
  ; Procedimiento: copy_rel_str
  ; Descripcion: Copia un caracter de un espacio en memoria a otro desplazado
  ; Entrada: BX = Apuntador, CX (CL | CH) = desplazamientos
  ; Salida: AL = Caracter copia
  ; =====================================================================
  copy_rel_str proc
    ; -- Imprime lo contenido en BX aumentando el recorrido del contador I
    ; -- Almacena en AL lo contenido en el recorrido BX
    push bx
    push cx
    xor cx, cx
    mov cl, dl
    call inc_bx_cx
    mov al, byte ptr [bx] ; AL contiene el caracter a copiar
    call putchar
    pop cx
    pop bx
    ; #######################
    push bx
    call inc_bx_cx
    mov byte ptr [bx], al
    inc ch
    pop bx
    ret
  copy_rel_str endp


  ; =====================================================================
  ; Procedimiento: invert_al (Simular boolean)
  ; Descripcion: Invierte el valor que contiene AL (0 -> 1 | 1 -> 0)
  ; Entrada: AL = Valor a invertir
  ; Salida: AL = Valor invertido
  ; =====================================================================
  invert_al proc
    cmp al, 1
    je p_is_one_invert

    mov al, 1
    jmp return_invert

    p_is_one_invert:
    xor al, al

    return_invert: ret
  invert_al endp

  ; =====================================================================
  ; Procedimiento: FL_ROW (First and Last ROW)
  ; Descripcion: -
  ; Entrada: -
  ; Salida: -
  ; =====================================================================
  fl_row proc
    call copy_rel_str
    ; -- Impresion de espacios
    push cx
    xor cx, cx
    mov cl, ah
    add cl, cl
    dec cl
    call str_blank
    pop cx
    ; -- Modificacion de I (DL)
    add dl, ah
    add dl, ah
    dec dl
    dec dl
    ret
  fl_row endp

  ; =====================================================================
  ; Procedimiento: INC_BX_CX
  ; Descripcion: Incrementa bx con los sub-registros de CX (CL + CH)
  ; Entrada: CX = Desplazamiento requerido
  ; Salida: BX = Desplazamiento designado por CL + CH
  ; =====================================================================
  inc_bx_cx proc
    push cx
    add cl, ch
    
    p_sum_cl:
      cmp cl, 0
      je p_end_inc
      inc bx
      dec cl
      jmp p_sum_cl

    p_end_inc:
    pop cx
    ret
  inc_bx_cx endp

  ; #####################################################################
  ; #####################################################################
  ;                   HERRAMIENTAS Cadenas / String
  ; #####################################################################
  ; #####################################################################

  ; =====================================================================
  ; Procedimiento: STR_COPY (String Copy)
  ; Descripcion: 
  ; Entrada: BX = Direccion en memoria de la cadena origen
  ; Salida: DX = Direccion en memoria de la cadena copia
  ; =====================================================================
  str_copy proc
    push bx
    push cx
    push dx

    p_copy:
      mov cl, byte ptr [bx]
      push bx
      mov bx, dx
      mov byte ptr [bx], cl
      pop bx
      inc bx
      inc dx
      cmp byte ptr [bx], 0
      jne p_copy

    mov bx, dx
    mov byte ptr [bx], 0

    pop dx
    pop cx
    pop bx
    ret
  str_copy endp

  ; =====================================================================
  ; Procedimiento: STR_LEN (String Length)
  ; Descripcion: Procedimiento que cuenta la cantidad de caracteres de una
  ;              cadena almacenada en memoria, compara el fin de cadena = 0.
  ; Entrada: CX = Direccion en memoria de la cadena
  ; Salida: CL = Longitud de la palabra
  ; =====================================================================
  str_len proc
    push bx

    mov bx, cx
    xor cx, cx
    p_count:
      inc bx
      inc cl

      cmp byte ptr [bx], 0
      jne p_count

    pop bx
    ret
  str_len endp

  ; =====================================================================
  ; Procedimiento: STR_ENTER (String Enter)
  ; Descripcion: Procedimiento para realizar un salto de linea y un corrimiento
  ;              de carro utilizando el procedimeinto putchar.
  ; Entrada: -
  ; Salida: -
  ; =====================================================================
  str_enter proc
    push ax

    mov al, 10
    call putchar

    mov al, 13
    call putchar

    pop ax
    ret
  str_enter endp

  ; =====================================================================
  ; Procedimiento: STR_BLANK (String Blank)
  ; Descripcion: Procedimiento para imprimir N cantidad de espacios en blanco.
  ; Entrada: CX = Cantidad de espacios
  ; Salida: -
  ; =====================================================================
  str_blank proc
    push ax
    
    p_blank:
      mov al, ' '
      call putchar
      loop p_blank

    pop ax
    ret
  str_blank endp

  ; #####################################################################
  ; #####################################################################
  ;                   HERRAMIENTAS Entrada / Salida
  ; #####################################################################
  ; #####################################################################

  ; =====================================================================
  ; Procedimiento: GETCHAR
  ; Descripcion: Procedimiento que lee un caracter del teclado haciendo
  ;              uso de la int 21h servicio 08h del S.O. ** SIN ECHO **
  ; Entrada: -
  ; Salida: AL = Caracter tecleado
  ; =====================================================================
  getchar proc
    mov ah, 08
    int 21h
    ret
  getchar endp

  ; =====================================================================
  ; Procedimiento: PUTCHAR
  ; Descripcion:
  ; Entrada: -
  ; Salida: -
  ; =====================================================================
  putchar proc
    push ax
    push dx
    
    mov dl, al
    mov ah, 2
    int 21h
    
    pop dx
    pop ax
    ret
  putchar endp

  ; =====================================================================
  ; Procedimiento: PUTS
  ; Descripcion: Procedimiento para impresion de cadenas utilizando el procedimeinto
  ;              putchar, deteniendose al encontrar el fin de cadena = 0
  ; Entrada: DX = direccion de memoria que contiene la cadena (offset)
  ; Salida: -
  ; =====================================================================
  puts proc
    push ax
    push bx
    
    mov bx, dx
    @@nxt:
      mov al, [bx]
      cmp al, 0
      je @@end
      call putchar
      inc bx
      jmp @@nxt
    
    @@end:
      pop bx
      pop ax
      ret
  puts endp

endp
end PRINCIPAL
