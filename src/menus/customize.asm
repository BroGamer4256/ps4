%include "cmn.asm"

extern implOfLoadModuleChoiceList
extern whereLoadModuleChoiceList
extern realLoadModuleChoiceList

extern implOfLoadHairstyleChoiceList
extern whereLoadHairstyleChoiceList
extern realLoadHairstyleChoiceList

section .text
strlen:
	mov r8d, -1
	dec rax
.loop:
	inc r8d
	inc rax
	cmp byte [rax], 0
	jne .loop
	ret

implOfLoadModuleChoiceList:
	push rax
	push rbx
	push rcx
	push rsi
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	sub rsp, 0x200

	mov rcx, rdi
	mov r8, rbx
	call realLoadModuleChoiceList
	mov rdx, rax
	call strlen

	add rsp, 0x200
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop rsi
	pop rcx
	pop rbx
	pop rax

	mov r9, [rel whereLoadModuleChoiceList]
	add r9, 6 + 7
	jmp r9

section .text
implOfLoadHairstyleChoiceList:
	push rax
	push rbx
	push rcx
	push rsi
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	sub rsp, 0x200

	mov rcx, rdi
	mov r8, rbx
	call realLoadHairstyleChoiceList
	mov rdx, rax
	call strlen

	add rsp, 0x200
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop rsi
	pop rcx
	pop rbx
	pop rax

	mov r9, [rel whereLoadHairstyleChoiceList]
	add r9, 6 + 7
	jmp r9
