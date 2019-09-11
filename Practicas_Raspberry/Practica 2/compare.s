	.text
	.global _start

_start:
	mov	r1,#1
	mov	r2,#9
	svc	#0
	cmp	r1,r2
	beq	igual
	bmi	menor
	bpl	mayor
	bx	lr

mayor:
	mov 	r7, #4
	mov 	r0, #1
	ldr 	r1,=es_mayor
	mov 	r2, #9
	svc	#0
	mov	r7,#1
	mov	r0,#0
	svc	#0

menor:
	mov 	r7, #4
	mov 	r0, #1
	ldr 	r1,=es_menor
	mov 	r2, #9
	svc	#0
	mov	r7,#1
	mov	r0,#0
	svc	#0

igual:
	mov	r7, #4
	mov	r0, #1
	ldr 	r1,=es_igual
	mov	r2, #9
	svc	#0
	mov	r7,#1
	mov	r0,#0
	svc	#0

	.data

es_mayor:
	.ascii "#1 > #2\n"
es_menor:
	.ascii "#1 < #2\n"
es_igual:
	.ascii "#1 = #2\n"
