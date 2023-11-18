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

extern implOfGetBgmIndexCsResult1
extern whereGetBgmIndexCsResult1

extern implOfGetBgmIndexCsResult2
extern whereGetBgmIndexCsResult2

extern implOfGetBgmIndexCsResult3
extern whereGetBgmIndexCsResult3

extern implOfGetBgmIndexCsResult4
extern whereGetBgmIndexCsResult4

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
	mov rax, [rel whereGetBgmIndexCsMenu]
	mov r8d, [r14 + 0x5D28]
	cmp r8d, 2
	jg .skip
	mov edx, [r14 + 0x5D2C]
	add rax, 0x07
	jmp rax
.skip:
	add rax, 0x18
	jmp rax

implOfGetBgmIndexCsGalleryIn:
	mov rdx, [rel whereGetBgmIndexCsGalleryIn]
	jmp GetBgmIndexCsGallery
implOfGetBgmIndexCsGalleryLoop:
	mov rdx, [rel whereGetBgmIndexCsGalleryLoop]
	jmp GetBgmIndexCsGallery
implOfGetBgmIndexCsGalleryOut:
	mov rdx, [rel whereGetBgmIndexCsGalleryOut]
	jmp GetBgmIndexCsGallery
GetBgmIndexCsGallery:
	cmp r8d, 2
	jg .skip
	mov r9d, 0x0B
	add rdx, 0x06
	jmp rdx
.skip:
	add rdx, 0x3A
	jmp rdx

implOfGetBgmIndexCsResult1:
	mov rax, [rel whereGetBgmIndexCsResult1]
	mov rcx, 0x45
	mov rdx, 0x06
	mov r9d, 0x06
	jmp GetBgmIndexCsResult
implOfGetBgmIndexCsResult2:
	mov rax, [rel whereGetBgmIndexCsResult2]
	mov rcx, 0x45
	mov rdx, 0x06
	mov r9d, 0x06
	jmp GetBgmIndexCsResult
implOfGetBgmIndexCsResult3:
	mov rax, [rel whereGetBgmIndexCsResult3]
	mov rcx, 0x49
	mov rdx, 0x0D
	mov r9d, 0x06
	mov r8, [rdi + r8 * 0x8 + 0xBE9C50]
	jmp GetBgmIndexCsResult
implOfGetBgmIndexCsResult4:
	mov rax, [rel whereGetBgmIndexCsResult4]
	mov rcx, 0x45
	mov rdx, 0x06
	mov r9d, 0x05
	jmp GetBgmIndexCsResult
GetBgmIndexCsResult:
	cmp r8, 2
	jg .skip
	add rax, rdx
	jmp rax
.skip:
	add rax, rcx
	jmp rax
