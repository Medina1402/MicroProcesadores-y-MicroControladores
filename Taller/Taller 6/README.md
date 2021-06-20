En el lenguaje ensamblador de AVR existe tanto la directiva MACRO como ENDMACRO (o ENDM). La directiva MACRO le indica al programa ensamblador (programa traductor) que es el inicio de la definición de una macro-instrucción y toma el siguiente símbolo como el nombre de la macro instrucción. En en caso de la directiva ENDMACRO o ENDM se utiliza para indicar el fin de la definición de la  macro-instrucción

Ejemplo:
```assembly
    .MACRO SUMAR ;Inicio de la definición
        ADD R15, R16
        ADD R15, R17
    .ENDM        ;Fin de la definición
```

Para hacer uso de una macro-instrucción ya definida simplemente se escribe el nombre de la macro-instrucción más adelante en el programa y entonces en el proceso de traducción (ensamblaje) la definición de la macro se expande en todos lugar donde se haya utilizados.

## Parámetros en macro-instrucción en AVR
Una macro-instrucción puede tomar hasta 10 parámetros. Estos parámetros se conocen dentro de la definición como @0 a @9. Al invocar una macro-instrucción con parámetros, éstos se proporcionan como una lista separada por comas.

Ejemplo:
```assembly
    .MACRO SUMA_REGS ;Inicio de la definición
        ADD @0, @1
        ADD @0, @2
        ADD @0, @3
    .ENDM        ;Fin de la definición
```

Uso:
```assembly
    SUMAR_REGS R15, R16, R17, R18
```

Se expande a:
```assembly
    ADD R15, R16
    ADD R15, R17
    ADD R15, R18
```