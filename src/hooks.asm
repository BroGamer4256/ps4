extern implOfCmnMenuTouchCheck
extern AppendLayerSuffix
extern GetLayerFrame
extern _ZN4diva15PlaySoundEffectE
extern whereCmnMenuTouchCheck

extern implOfDecoPlaceholderApply
extern _ZN10decoration16placeholderApplyEPN4diva12AetLayerArgsEPNS0_12AetLayerDataE
extern whereDecoPlaceholderApply

section .text
implOfCmnMenuTouchCheck:
	cmp edi, eax
	jz implOfCmnMenuTouchCheckExit
	mov [rbx], edi

	push rax
	push rcx
	push rdx
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	lea rdx, [rbp - 0x51]
	mov rcx, rbx
	mov dword [rcx + 0xAC], 1
	call [rel AppendLayerSuffix]

	mov rdx, rax
	; std::string to char *
	cmp qword [rdx + 0x18], 0x10
	cmovnc rdx, [rdx]
	mov ecx, [rbx + 0x1C]
	call [rel GetLayerFrame]
	movss [rbx + 0x90], xmm0
	movss [rbx + 0x94], xmm0

	lea rcx, [rbx + 0x58]
	; std::string to char *
	cmp qword [rcx + 0x18], 0x10
	cmovnc rcx, [rcx]
	mov eax, __?float32?__(1.0)
	movd xmm1, eax
	call [rel _ZN4diva15PlaySoundEffectE]

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdx
	pop rcx
	pop rax
implOfCmnMenuTouchCheckExit:
	mov edx, eax
	mov rax, [rel whereCmnMenuTouchCheck]
	add rax, 7

	cmp edi, edx
	mov edi, 1

	jmp rax

implOfDecoPlaceholderApply:
	push rsp
	push rax
	push rbx
	push rcx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	sub rsp, 16
	mov rcx, rsi
	mov rdx, rax
	call _ZN10decoration16placeholderApplyEPN4diva12AetLayerArgsEPNS0_12AetLayerDataE
	add rsp, 16

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rcx
	pop rbx
	pop rax
	pop rsp

	movups xmm0, [rax]
	movups [rbx - 0x14], xmm0

	mov rdx, [rel whereDecoPlaceholderApply]
	add rdx, 7
	jmp rdx
