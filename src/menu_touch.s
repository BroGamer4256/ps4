.global implOfCmnMenuTouchCheck

implOfCmnMenuTouchCheck:
	cmp edi, eax
	jz exit
	mov [rbx], edi

	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
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
	mov dword ptr [rcx + 0xAC], 1
	movabs rax, 0x14022D070
	call rax

	mov rdx, rax
	cmp qword ptr [rdx + 0x18], 0x10
	cmovnc rdx, [rdx]
	mov ecx, [rbx + 0x1C]
	movabs rax, 0x1402CA120
	call rax
	movss [rbx + 0x90], xmm0
	movss [rbx + 0x94], xmm0

	lea rdx, [rbx + 0x58]
	cmp qword ptr [rdx + 0x18], 0x10
	cmovnc rdx, [rdx]
	mov rax, 0x3F800000
	movd xmm2, rax
	mov rcx, 1
	movabs rax, 0x1405aa550
	call rax

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
exit:
	cmp edi, eax
	mov edi, 1

	movabs rcx, 0x14022C597
	jmp rcx
