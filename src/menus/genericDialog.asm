%include "cmn.asm"

extern implOfSetKeyAnmOpacity
extern whereSetKeyAnmOpacity
extern realSetKeyAnmOpacity

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
