%include "cmn.asm"

extern implOfSetKeyAnmOpacity
extern whereSetKeyAnmOpacity
extern realSetKeyAnmOpacity

extern implOfPlayGenericDialogOut
extern wherePlayGenericDialogOut
extern originalPlayGenericDialogOut
extern realPlayGenericDialogOut

extern implOfSetHelpPageNo10Color
extern whereSetHelpPageNo10Color

extern implOfSetHelpPageNo01Color
extern whereSetHelpPageNo01Color

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

implOfSetHelpPageNo10Color:
	mov rax, [rel whereSetHelpPageNo10Color]
	add rax, 7
	jmp SetHelpPageNoColor

implOfSetHelpPageNo01Color:
	mov rax, [rel whereSetHelpPageNo01Color]
	add rax, 7
	jmp SetHelpPageNoColor

SetHelpPageNoColor:
	movss xmm0, dword [rbx + 0x60]
	mov ecx, __?float32?__(255.0)
	movd xmm1, ecx
	mulss xmm0, xmm1
	cvttss2si ecx, xmm0
	shl ecx, 0x18
	or ecx, 0xFFFFFF
	mov [rbp + 0x8], ecx

	jmp rax
