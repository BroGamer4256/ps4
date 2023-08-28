%include "cmn.asm"

extern implOfLoadModuleChoiceList
extern whereLoadModuleChoiceList
extern realLoadModuleChoiceList

extern implOfLoadHairstyleChoiceList
extern whereLoadHairstyleChoiceList
extern realLoadHairstyleChoiceList

extern implOfSetModuleChoiceListPriority
extern whereSetModuleChoiceListPriority
extern implOfSetHairstyleChoiceListPriority
extern whereSetHairstyleChoiceListPriority

extern implOfSetModuleSprPriority
extern whereSetModuleSprPriority
extern implOfSetHairstyleSprPriority
extern whereSetHairstyleSprPriority

extern implOfSetModuleSelectedPriority
extern whereSetModuleSelectedPriority
extern implOfSetHairstyleSelectedPriority
extern whereSetHairstyleSelectedPriority

extern implOfMemset
extern originalMemset

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
	mov rdx, rsi
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
	mov rdx, r14
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

implOfSetModuleChoiceListPriority:
	mov rax, [rel whereSetModuleChoiceListPriority]
	jmp SetChoiceListPriority
implOfSetHairstyleChoiceListPriority:
	mov rax, [rel whereSetHairstyleChoiceListPriority]
SetChoiceListPriority:
	mov r9d, ebx
	cmp r9d, 5
	jle .lesser
	neg r9d
	add r9d, 10
.lesser:
	imul r9d, 2
	add r9d, 9

	add rax, 6
	jmp rax

implOfSetModuleSprPriority:
	mov rdx, [rel whereSetModuleSprPriority]
	add rdx, 5 + 3 + 3
	mov eax, r11d
	jmp SetSprPriority
implOfSetHairstyleSprPriority:
	mov rdx, [rel whereSetHairstyleSprPriority]
	add rdx, 5 + 4 + 3 + 3
	mov eax, esi
	mov esi, [rsp+0x20]
SetSprPriority:
	cmp eax, 5
	jle .lesser
	neg eax
	add eax, 10
.lesser:
	imul eax, 2
	add eax, 10

	jmp rdx

implOfSetModuleSelectedPriority:
	mov r8, [rel whereSetModuleSelectedPriority]
	add r8, 6 + 4 + 2
	jmp SetSelectedPriority
implOfSetHairstyleSelectedPriority:
	mov r8, [rel whereSetHairstyleSelectedPriority]
	add r8, 6 + 5
SetSelectedPriority:
	mov r9d, r15d
	cmp r9d, 5
	jl .lesser
	sub r9d, 5
.lesser:
	imul r9d, 2
	add r9d, 10

	jmp r8

implOfMemset:
	push r11
	call [rel originalMemset]
	pop r11
	ret
