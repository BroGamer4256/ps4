%include "cmn.asm"

extern implOfSetKeyAnmOpacity
extern whereSetKeyAnmOpacity
extern realSetKeyAnmOpacity

extern implOfPlayGenericDialogOut
extern wherePlayGenericDialogOut
extern originalPlayGenericDialogOut
extern realPlayGenericDialogOut

section .text
implOfSetKeyAnmOpacity:
	pushaq
	mov rcx, rsi
	sub rsp, 0x10
	call realSetKeyAnmOpacity
	add rsp, 0x10
	movd [rbp - 0x58], xmm0
	popaq
	mov rax, [rel whereSetKeyAnmOpacity]
	add rax, 7
	jmp rax

implOfPlayGenericDialogOut:
	pushaq
	mov rcx, rbx
	sub rsp, 0x10
	call realPlayGenericDialogOut
	add rsp, 0x10
	test al, 0
	popaq
	je .skip
	call [rel originalPlayGenericDialogOut]
	ret
.skip:
	mov rcx, [rel wherePlayGenericDialogOut]
	add rcx, 0x11
	jmp rcx
