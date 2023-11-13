%include "cmn.asm"

extern AppendLayerSuffix
extern GetLayerFrame
extern _ZN4diva15PlaySoundEffectE

extern implOfCmnMenuTouchCheck
extern whereCmnMenuTouchCheck

extern implOfCtrlLayer
extern CtrlLayerReturn
extern CtrlLayerContinue

extern implOfGetBgmIndexCsMenu
extern whereGetBgmIndexCsMenu

extern implOfGetBgmIndexCsGalleryIn
extern whereGetBgmIndexCsGalleryIn

extern implOfGetBgmIndexCsGalleryLoop
extern whereGetBgmIndexCsGalleryLoop

extern implOfGetBgmIndexCsGalleryOut
extern whereGetBgmIndexCsGalleryOut

section .text
implOfCmnMenuTouchCheck:
	cmp edi, eax
	jz .exit
	mov [rbx], edi

	pushaq

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

	popaq
.exit:
	mov edx, eax
	mov rax, [rel whereCmnMenuTouchCheck]
	add rax, 7

	cmp edi, edx
	mov edi, 1

	jmp rax

implOfCtrlLayer:
	mov ax, 0x8000
	and ax, [rdx + 0x18]
	jne .played
	roundss xmm2, xmm2, 3
	xorps xmm0, xmm0
	ucomiss xmm2, xmm0
	jne .played
	or word [rdx + 0x18], 0x8000
	mov rax, [rel CtrlLayerContinue]
	jmp rax
.played:
	mov rax, [rel CtrlLayerReturn]
	jmp rax

implOfGetBgmIndexCsMenu:
	mov eax, 0
	cmp r8d, 2
	cmovg r8d, eax
	mov [r14 + 0x5d28], r8b
	mov rax, [rel whereGetBgmIndexCsMenu]
	add rax, 7
	jmp rax

implOfGetBgmIndexCsGalleryIn:
	mov edx, 0
	cmp r8d, 2
	cmovg r8d, edx
	mov r9d, 0x0B
	mov rdx, [rel whereGetBgmIndexCsGalleryIn]
	add rdx, 6
	jmp rdx

implOfGetBgmIndexCsGalleryLoop:
	mov edx, 0
	cmp r8d, 2
	cmovg r8d, edx
	mov r9d, 0x0B
	mov rdx, [rel whereGetBgmIndexCsGalleryLoop]
	add rdx, 6
	jmp rdx

implOfGetBgmIndexCsGalleryOut:
	mov edx, 0
	cmp r8d, 2
	cmovg r8d, edx
	mov r9d, 0x0B
	mov rdx, [rel whereGetBgmIndexCsGalleryOut]
	add rdx, 6
	jmp rdx
