;===========================================================================
; klib.asm 
;   Copyright (C) 2012 Free Software Foundation, Inc.
;   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
;
;This file is part of DTHAS.
;
;DTHAS is free software; you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation; either version 2 of the License, or 
;(at your option) any later version.
;
;DTHAS is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
;===========================================================================

%include "krl.h"
%include "gdt.h"

[section .text]

extern	pos
extern	gdt_desc

global	disp_num
global	disp_sym
global	disp_dword
global	disp_str
global	out_32
global	in_32
global	in_16
global	out_8
global	out_16
global	in_8
global	mcopy
global	en_irq
global	dis_irq
global	en_int
global	dis_int
global	desc_setting
global	desc_baseaddr_get
global	phycopy
global	gethddata
global	puthddata
global	sys_enpage
global	page_set
global	upd_gdt_descs_user

global	reboot
global	shutdown

;=======================================================================
disp_num:
; disp_num(int num)
; eax is parameter which is the value need to be displayed
;=======================================================================
	push	eax
	push	ebx
	push	ecx

	mov	eax, dword[esp + 16]		; value to be displayed

	mov	ebx, eax			; save eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '0'
	push	eax
	call	disp_sym			; '0'
	pop	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'x'
	push	eax
	call	disp_sym			; 'x'
	pop	eax

	mov	eax, ebx			; restore eax
	
	mov	ebx, eax
	and	ebx, 0xF0000000
	mov	cl, 28
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x0F000000
	mov	cl, 24
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x00F00000
	mov	cl, 20
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x000F0000
	mov	cl, 16
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x0000F000
	mov	cl, 12
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x00000F00
	mov	cl, 8
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x000000F0
	mov	cl, 4
	shr	ebx, cl	
	call	disp_num_2

	mov	ebx, eax
	and	ebx, 0x0000000F
	call	disp_num_2

	pop	ecx
	pop	ebx
	pop	eax	

	ret


;=======================================================================
disp_num_2:
; ebx is parameter which is the value need to be displayed
;=======================================================================

	push	eax
	push	esi	

	mov	esi, dword[pos]

	mov	eax, ebx

	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	ah, 0x7					; for disp

	mov	[gs:esi], ax
	add	esi, 2
	
	jmp	.end

;a~f
.2:
	add	al, 0x57
	mov	ah, 0x7					; for disp

	mov	[gs:esi], ax
	add	esi, 2

.end:
	mov	dword[pos], esi

	pop	esi
	pop	eax

	ret


;=======================================================================
disp_sym:
; disp_sym(int sym)
;=======================================================================
	push	eax
	push	ebx
	
	;mov	eax, dword[esp + 4]			; value to be displayed
	mov	eax, dword[esp + 12]			; value to be displayed
	
	cmp	al, 32
	jb	.end

	cmp	al, 126
	ja	.end

	mov	ebx, dword[pos]

	mov	[gs:bx], ax
	
	add	ebx, 2
	mov	dword[pos], ebx


.end:

	pop	ebx
	pop	eax

	ret


;=======================================================================
disp_str
; disp_str(char *str)
;=======================================================================
		
	push	eax
	push	ebp

	;mov	ebp, dword[esp + 4]		; address of str
	mov	ebp, dword[esp + 12]		; address of str

.0:
	xor	eax, eax
	mov	al, byte[ebp]

	cmp	al, 0
	je	.40
	cmp	al, 32
	jb	.40
	cmp	al, 126
	ja	.40
	
	mov	ah, 0x7 
	push	eax
	call	disp_sym
	pop	eax

	add	ebp, 1

	jmp	.0

.40:	
	pop	ebp
	pop	eax
	

	ret


	
;=======================================================================
disp_dword:
; disp_dword(int value);
;=======================================================================
	push	eax
	push	ebx
	push	ecx

	mov	eax, dword[esp + 16]		; value to be displayed

	mov	ebx, eax

	mov	eax, ebx
	and	eax, 0xFF000000
	mov	cl, 24
	shr	eax, cl	

	cmp	al, 32
	jb	.10
	cmp	al, 126
	ja	.10

	mov	ah, 0x7 
	push	eax
	call	disp_sym
	pop	eax

.10:
	mov	eax, ebx
	and	eax, 0x00FF0000
	mov	cl, 16
	shr	eax, cl

	cmp	al, 32
	jb	.20
	cmp	al, 126
	ja	.20
	
	mov	ah, 0x7 
	push	eax
	call	disp_sym
	pop	eax

.20:
	mov	eax, ebx
	and	eax, 0x0000FF00
	mov	cl, 8
	shr	eax, cl

	cmp	al, 32
	jb	.30
	cmp	al, 126
	ja	.30
	
	mov	ah, 0x7 
	push	eax
	call	disp_sym
	pop	eax

.30:
	mov	eax, ebx
	and	eax, 0x000000FF

	cmp	al, 32
	jb	.40
	cmp	al, 126
	ja	.40

	mov	ah, 0x7 
	push	eax
	call	disp_sym
	pop	eax

.40	
	pop	ecx
	pop	ebx
	pop	eax	

	ret


; ========================================================================
out_32:
;void out_32(u16 port, u32 value);
; ========================================================================

	mov	edx, [esp + 4]			; port
	mov	eax, [esp + 4 + 4]		; send value
	out	dx, eax
	nop
	nop
	nop
	ret

; ========================================================================
in_32:
;u32 in_32(u16 port);
; ========================================================================

	mov	edx, [esp + 4]			; port
	xor	eax, eax			; return value
	in	eax, dx
	nop
	nop
	nop
	ret

;=======================================================================
in_16:
;u16 in_16(u16 port);
;=======================================================================
	mov	edx, dword[esp + 4]		; port
	xor	eax, eax			; return value

	in	ax, dx
	nop
	nop
	nop
	ret

;=======================================================================
out_16:
;out_16(u16 port, u16 value);
;=======================================================================
	mov	edx, dword[esp + 4]		; port
	mov	eax, dword[esp + 4 + 4]		; send value

	out	dx, ax
	nop
	nop
	nop
	ret

;=======================================================================
out_8:
;out_8(u16 port, u8 value);
;=======================================================================
	mov	edx, dword[esp + 4]		; port
	mov	al, byte[esp + 4 + 4]		; send value

	out	dx, al
	nop
	nop
	nop
	ret
;=======================================================================
in_8:
;u8 in_8(u16 port);
;=======================================================================
	mov	edx, dword[esp + 4]		; port
	xor	eax, eax			; return value

	in	al, dx
	nop
	nop
	nop
	ret

;=======================================================================
mcopy:
;void mcopy(u32 *dest, u32* src, u16 len);
;edi: dest addr; esi: src addr; ecx: len
;=======================================================================

	mov	edi, [esp + 4]		; dest address
	mov	esi, [esp + 8]		; src  address
	mov	ecx, [esp + 12]		; length
	
.0:
	mov	al, byte[ds:esi]
	mov	byte[ds:edi], al

	add	esi, 1
	add	edi, 1
	
	sub	cx, 1

	cmp	cx, 0
	jne	.0

	mov	eax, 0			; return value

	ret

;=======================================================================
en_irq:
;PUBLIC void en_irq(u32 irq)
;=======================================================================

	push	ecx

	mov	ecx, [esp + 8]		; irq
	
	pushf	

	cli
	
	cmp	ecx, 7
	ja	.10

.0:
	mov	bx, 1
	shl	bx, cl			; for example, irq = 3, then bl = 00001000
	
	mov	bh, 0xFF		; bh = 11111111
	sub	bh, bl			; bh = 11111111 - 00001000 = 11110111

	in	al, PRIMARY_PORT_21
	and	al, bh			; bit 3(irq = 3) is cleared to 0
	out	PRIMARY_PORT_21, al

	jmp	.end


.10:
	sub	ecx, 8			; for example, irq = 11, then ecx = 11 - 8 = 3

	mov	bx, 1
	shl	bx, cl			; bl = 00001000
	
	mov	bh, 0xFF		; bh = 11111111
	sub	bh, bl			; bh = 11111111 - 00001000 = 11110111

	in	al, SLAVE_PORT_A1
	and	al, bh			; bit 3(irq = 3) is cleared to 0
	out	SLAVE_PORT_A1, al

	jmp	.end

.end:
	popf
	
	pop	ecx
	
	ret

;=======================================================================
dis_irq:
;PUBLIC void dis_irq(u32 irq)
;=======================================================================

	push	ecx

	mov	ecx, [esp + 8]		; irq
	
	pushf	

	cli
	
	cmp	ecx, 7
	ja	.10

.0:
	mov	bx, 1
	shl	bx, cl			; for example, irq = 3, then bl = 00001000
	
	in	al, PRIMARY_PORT_21
	or	al, bh			; bit 3(irq = 3) is set to 1
	out	PRIMARY_PORT_21, al

	jmp	.end


.10:
	sub	ecx, 8			; for example, irq = 11, then ecx = 11 - 8 = 3

	mov	bx, 1
	shl	bx, cl			; bl = 00001000
	
	in	al, SLAVE_PORT_A1
	or	al, bh			; bit 3(irq = 3) is cleared to 0
	out	SLAVE_PORT_A1, al

	jmp	.end

.end:
	popf
	
	pop	ecx
	
	ret

;=======================================================================
dis_int:
;PUBLIC void dis_int()
;=======================================================================
	cli
	ret

;=======================================================================
en_int:
;PUBLIC void en_int()
;=======================================================================
	sti
	ret



;===============================================================
desc_setting:
;eax: base addr; ebx: limit; edi: attribute; ebp: GDT index
;===============================================================

	push	ecx
	push 	edx
	
	mov	word[ebp + 0], bx	; 0, 1; limit1

	mov	edx, eax
	and	edx, 0xFFFFFF		; base1
	mov	word[ebp + 2], dx	; 2, 3
	mov	cl, 16
	shr	edx, cl
	mov	byte[ebp + 4], dl	; 4

	mov	edx, ebx
	mov	cl, 16
	shr	edx, cl
	mov	cl, 8
	shl	dx, cl			; limit2
	or	dx, di			; attribute
	mov	word[ebp + 5], dx	; 5, 6

	mov	cl, 24
	shr	eax, cl			; base2
	mov	byte[ebp + 7], al	; 7
		
	pop 	edx
	pop	ecx

	ret



;===============================================================
desc_baseaddr_get:
;PUBLIC u32 desc_baseaddr_get(u32 addr)
;eax: base addr(return); ebp: addr
;===============================================================

	push	ecx
	push 	edx
	push	ebp
	
	xor	eax, eax

;ebp = gdt_desc + PROC_SYS_FIRST
	mov	ecx, DESC_SYS_FIRST
	mov	ebp, gdt_desc
	add	ebp, ecx	
	
;ebp = gdt_desc + PROC_SYS_FIRST + index * 24 bytes
;	mov	eax, [esp + 4]		; index
	mov	eax, [esp + 16]		; index, at the begining, there are 3 push, which is 4 bytes each
	mov	cl, 24			; 8 * 3
	mul	cl
	add	ebp, eax

	xor	edx, edx

	mov	al, byte[ebp + 7]	; base3
	mov	cl, 24
	shl	eax, cl

	mov	dl, byte[ebp + 4]	; base2
	mov	cl, 16
	shl	edx, cl
	or	eax, edx

	mov	ax, word[ebp + 2]	; base1

	pop	ebp
	pop 	edx
	pop	ecx

	ret


;===============================================================
phycopy:
;PUBLIC void phycopy(u8* dst, u8* src, u32 size)
;===============================================================

	push	eax
	push	ebx
	push	ecx
	push 	esi
	push	edi
	
	mov	edi, [esp + 24]		; 1th param, dst
	mov	esi, [esp + 28]		; 2th param, src
	mov	ecx, [esp + 32]		; 3th param, size

	xor	ebx, ebx

.10:
	mov	al, byte[esi + ebx]
	mov	byte[edi + ebx], al
	add	ebx, 1
	
	cmp	ebx, ecx
	jb	.10

	pop	edi
	pop	esi
	pop	ecx
	pop	ebx
	pop	eax

	ret


;===============================================================
gethddata:
;PUBLIC s32 gethddata(s16 * buf, u32 total_size)
;===============================================================

	push	ebx
	push	ecx
	push	edx
	push	edi
	
	mov	edi, [esp + 20]		; 1th param, buf
	mov	ecx, [esp + 24]		; 2th param, total_size(bytes)

	xor	ebx, ebx
	xor	eax, eax

	mov	edx, 0x1F0

.20:
	in	ax, dx

	mov	word[edi + ebx], ax

	add	ebx, 2
		
	cmp	ebx, ecx
	jb	.20

	mov	eax, ebx

	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	
	ret

;===============================================================
puthddata:
;PUBLIC s32 puthddata(s16 * buf, u32 total_size)
;===============================================================

	push	ebx
	push	ecx
	push	edx
	push	edi
	
	mov	edi, [esp + 20]		; 1th param, buf
	mov	ecx, [esp + 24]		; 2th param, total_size(bytes)

	xor	ebx, ebx
	xor	eax, eax

	mov	edx, 0x1F0

.20:
	mov	ax, word[edi + ebx]
	out	dx, ax	

	add	ebx, 2
		
	cmp	ebx, ecx
	jb	.20

	mov	eax, ebx

	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	
	ret

;=======================================================================
sys_enpage:
;PUBLIC	void	sys_enpage(u32 base_addr);
;=======================================================================
	push	edx

	mov	edx, dword[esp + 8]		; base_addr
	mov	cr3, edx

	pop	edx

	ret

;===============================================================
page_set:
;PUBLIC	void	page_set(u32 page_dir_base, u32 page_table_base, u32 new_proc_base);
;===============================================================
	
	push	ebp
	push	ecx
	push 	ebx
	push	eax

	mov	eax, [esp + 24]		; page_table_base
	or	eax, PG_EXIST
	or	eax, PG_RW_W
	or	eax, PG_US_USER

	mov	ebp, [esp + 20]		; page_dir_base
	mov	ecx, 1024

;-------------------------------------------------------------
;Init page dir
;-------------------------------------------------------------
.0:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4
	
	sub	ecx, 1
	cmp	ecx, 0
	jne	.0


;-------------------------------------------------------------
;Init page table
;-------------------------------------------------------------
	mov	ebp, [esp + 24]			; page_table_base

	mov	ecx, USER_TABLE_PAGE_COUNT	; 1024 * 1024 / 4 * 3

	xor	eax, eax			; from physical address 0
	
	or	eax, PG_N_EXIST

	or	eax, PG_RW_W
	or	eax, PG_US_USER


	mov	ebx, [esp + 28]			; new_proc_base
	or	eax, ebx


	
.10:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4

	sub	ecx, 1
	cmp	ecx, 0
	jne	.10


;kernel space init
	
	;the value ebp is assigned before
 
	mov	ecx, KERNEL_TABLE_PAGE_COUNT	; 1024 * 1024 / 4 * 1

	xor	eax, eax			; from physical address 0
	or	eax, PG_EXIST
	or	eax, PG_RW_W
	or	eax, PG_US_USER
	or	eax, PG_PG
	or	eax, KERNEL_ADDR		; 0x100000
	
.20:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4

	sub	ecx, 1
	cmp	ecx, 0
	jne	.20

	pop	eax
	pop	ebx
	pop	ecx
	pop	ebp
		

	ret




;===============================================================
upd_gdt_descs_user:
;PUBLIC	void	upd_gdt_descs_user(u32 index, u32 proc_base);
;===============================================================
	push	ebp	
	push	esi
	push	edi
	push 	ecx
	push	eax	

	;init desc gdt_desc[14] ~ gdt_desc[128]
	mov	eax, [esp + 24]			; index
	mov	cl, 24				; each desc is 8 bytes, each proc has 3 desc
	mul	cl				; eax = index * 8 * 3
	
	mov	ecx, PROC_SYS_FIRST		; 0x28
	add	ecx, eax			; ecx = desc(index) = 0x28 + index * 8 * 3
	
	mov	esi, [esp + 28]			; proc_base

	;Init code desc
	mov	ebp, gdt_desc
	add	ebp, ecx	
	mov	eax, esi
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_CODE_EXE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	add	ecx, 8
	

	;Init data desc
	mov	ebp, gdt_desc
	add	ebp, ecx	
	mov	eax, esi
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	add	ecx, 8
	

	;Init stack desc
	mov	ebp, gdt_desc
	add	ebp, ecx	
	mov	eax, esi
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	

	pop	eax
	pop	ecx
	pop	edi
	pop	esi
	pop	ebp

	ret


;=======================================================================
reboot:
;PUBLIC void reboot()
;=======================================================================

	mov al, 0Eh
	mov dx, 0CF9h
	out dx, al
	jmp $

%include	"shutdown.asm"
