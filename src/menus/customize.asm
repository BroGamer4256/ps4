%include "cmn.asm"

extern implOfLoadChoiceList
extern whereLoadChoiceList
extern realLoadChoiceList

extern implOfFinishLoadChoiceList
extern whereFinishLoadChoiceList
extern realFinishLoadChoiceList

section .text
implOfLoadChoiceList:
	push rax
	push rcx
	push rsi
	push r8
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	sub rsp, 0x200 ; This shouldnt be neccessary but this dosent work without this

	mov rcx, rdi
	mov r8, rbx
	call realLoadChoiceList
	mov rdx, rax

	add rsp, 0x200
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r8
	pop rsi
	pop rcx
	pop rax

	mov r9, [rel whereLoadChoiceList]
	add r9, 7
	jmp r9
