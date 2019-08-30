	.text
	.global _start

_start:
	mov	r7,#4		@ Cargamos la constante 4 al registro 7
	mov	r0,#1		@ Cargamos la constante 1 al registro 0
	ldr	r1,=message	@ Cargamos el valor de la memoria al registro
	mov 	r2,#17		@ Cargamos la constante 14 al registro 2
	svc	#0		@ Supervisor de llamadas que guarda el contenido de los registros temporalmente
	mov 	r3, #1
	mov	r7,#1		@ Cargamos la constante 1 al registro 7
	mov	r0,#0		@ Cargamos la constante 0 al registro 0
	svc	#0		@ Supervisor de llamadas que gusrda el contenido de los registros temporalmente
       .data

message:
@       .ascii "Hola mundo\n"
	.ascii "FSE2020-1 is cool"
