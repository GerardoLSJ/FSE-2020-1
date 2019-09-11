	.text
	.global _start

_start:
	mov	r7,#4		@ r7 y r0 son utilizados por el sistema operativo
	mov	r0,#1		@ para determinar que funcion queremos llamar - imprimir -
				@ r1 y r2 son utilizados para pasar parametros a una funcion
	ldr	r1,=message	@ Se indica la ubicacion de la memoria del primer byte
				@ de la cadena que queremos imprimir
	mov 	r2,#17		@ Se indica el numero de caracteres de la cadena
	svc	#0		@ Se indica un interrupcion, ve los valores r0 y r7 y reconoce
				@ la solicitud de ejecucion de la funcion imprimir lo hace y
				@ regresa de la interrupcion
	mov	r7,#1		@ Ahora r7 y r0 son utilizados para indicarle al sistema
	mov	r0,#0		@ operativo que quiete el control del programa, es decir,
	svc	#0		@ salir del programa y  se indica de nuevo una interrupcion

       .data

message:
       .ascii "FSE2020-1 is cool\n"
