1. Ensamble el siguiente código (mnemónicos) a su correspondiente código (hexadecimal) de 8088:

| - | nemónicos | Binario | Hexadecimal |
| - | - | - | - |
| - | mov al, 83h | 1011 0000 1000 0011 | B0 |
| - | mov dx, 43h | 1011 1010 0100 0011 0000 0000 | BA43 |
| - | mov cx, 100h | 1001 1001 0000 0000 0000 0001 | B9000 |
| next: | mov di, 100h | 1001 1111 0000 0000 0000 0001 | BF00 |
| - | in al, dx | 1110 1100 | EC |
| - | mov [di], al | 1000 1000 0000 0101 | 8805 |
| - | inc di | 0100 0111 | 47 |
| - | loop next | 1110 0010 1111 1010 | E2FA |
| - | - | - | - |

2. Decodifique los siguientes opcode para un procesador 8088 utilizando los mnemónicos correspondientes:

| Dirección | Contenido | Binario | OpCode |
| - | - | - | - |
| 072A:100 | BE | 1011 1110 | - |
| 072A:101 | FF | 1111 1111 | - |
| 072A:102 | 0F | 0000 1111 | MOV SI, 0FFFh |
| 072A:103 | E5 | 1110 0101 | - |
| 072A:104 | 27 | 0010 0111 | IN AX, 27 |
| 072A:105 | 89 | 1000 1001 | - |
| 072A:106 | 04 | 0000 0100 | MOV WORD PTR [ SI ], AX |
| 072A:107 | 4E | 0100 1110 | DEC SI |
| 072A:108 | 75 | 0111 0101 | - |
| 072A:109 | F9 | 1111 1001 | JNE 103 |
| - | - | - | - |