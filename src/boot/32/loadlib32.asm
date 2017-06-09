;===========================================================================
; loadlib32.asm 
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

;=======================================================================
disp_num32:
; eax is parameter which is the value need to be displayed
;=======================================================================
	push	eax
	push	ebx
	push	ecx

	mov	ebx, eax			; save eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '0'
	call	disp_sym32			; '0'

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'x'
	call	disp_sym32			; 'x'

	mov	eax, ebx			; restore eax
	
	mov	ebx, eax
	and	ebx, 0xF0000000
	mov	cl, 28
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x0F000000
	mov	cl, 24
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x00F00000
	mov	cl, 20
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x000F0000
	mov	cl, 16
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x0000F000
	mov	cl, 12
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x00000F00
	mov	cl, 8
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x000000F0
	mov	cl, 4
	shr	ebx, cl	
	call	disp_num32_2

	mov	ebx, eax
	and	ebx, 0x0000000F
	call	disp_num32_2

	pop	ecx
	pop	ebx
	pop	eax	

	ret


;=======================================================================
disp_num32_2:
; ebx is parameter which is the value need to be displayed
;=======================================================================

	push	eax
	push	esi	

	mov	si, word[pos32]

	mov	eax, ebx

	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	ah, 0x7					; for disp

	mov	[gs:si], ax
	add	si, 2
	
	jmp	.end

;a~f
.2:
	add	al, 0x57
	mov	ah, 0x7					; for disp

	mov	[gs:si], ax
	add	si, 2

.end:
	mov	word[pos32], si

	pop	esi
	pop	eax

	ret


;=======================================================================
disp_sym32:
; eax is parameter which is the value need to be displayed
;=======================================================================
	
	push	ebx
	
	cmp	al, 32
	jb	.end

	cmp	al, 126
	ja	.end

	mov	bx, word[pos32]

	mov	[gs:bx], ax
	
	add	bx, 2
	mov	word[pos32], bx


.end:

	pop	ebx
		
	ret


;=======================================================================
disp_str32:
; eax is parameter which is the value need to be displayed
;=======================================================================
	push	eax
	push	ebx
	push	ecx

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
	call	disp_sym32

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
	call	disp_sym32

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
	call	disp_sym32

.30:
	mov	eax, ebx
	and	eax, 0x000000FF

	cmp	al, 32
	jb	.40
	cmp	al, 126
	ja	.40

	mov	ah, 0x7 
	call	disp_sym32

.40	
	pop	ecx
	pop	ebx
	pop	eax	

	ret


;===============================================================
fill_cmd32:
;parameter : ebp: the start sector ; ecx: sector count; edx: command
;===============================================================

;fill cmd
	mov	byte[cmd32+1], 0	; Features
	mov	byte[cmd32+2], cl	; Count

	mov	eax, ebp	; eax : the start sector
	mov	byte[cmd32+3], al	; LBA low

	mov	cl, 8
	shr	eax, cl
	mov	byte[cmd32+4], al	; LBA middle

	mov	eax, ecx
	mov	cl,16
	shr	eax, cl
	mov	byte[cmd32+5], al	; LBA high

	mov	eax, ecx
	mov	cl, 24
	shr	eax, cl
	and	al, 0xF
	or	al, 0xA0	; (lba_higest & 0xF | 0xA0) = (LOG_TAB_START >>24 & 0xF | 0xA0)
	or	al, 0x40	; lba<<6 = (1<<6)
	
	mov	byte[cmd32+6], al	; Device
	
	mov	byte[cmd32+7], dl	; Command

	call	cmd_send32

	ret

;=======================================================================
cmd_send32:
;=======================================================================

;send write to control block register
	mov	al, 0
	mov	dx, CBTP_PORT0_
	call	out_8_

;send command block registers
	
	mov	al, byte[cmd32+1]
	mov	dx, CBP_PORT1_
	call	out_8_

	mov	al, byte[cmd32+2]
	mov	dx, CBP_PORT2_
	call	out_8_

	mov	al, byte[cmd32+3]
	mov	dx, CBP_PORT3_
	call	out_8_

	mov	al, byte[cmd32+4]
	mov	dx, CBP_PORT4_
	call	out_8_

	mov	al, byte[cmd32+5]
	mov	dx, CBP_PORT5_
	call	out_8_

	mov	al, byte[cmd32+6]
	mov	dx, CBP_PORT6_
	call	out_8_

	mov	al, byte[cmd32+7]
	mov	dx, CBP_PORT7_
	call	out_8_

	ret

;=======================================================================
in_16_:
;(port:dx; value:al)
;=======================================================================
	in	ax, dx
	nop
	nop
	nop
	ret
;=======================================================================
out_8_:
;(port:dx; value:al)
;=======================================================================
	out	dx, al
	nop
	nop
	nop
	ret
