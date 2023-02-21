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
