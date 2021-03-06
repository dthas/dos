;===========================================================================
; loadlib.asm 
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
disp_msg:
;=======================================================================
	mov	bp, ax
	mov	ax, 01301h
	mov	bx, 0007h
	mov	dl, 0
	int	10h
		
	ret

;=======================================================================
clr_sc:
;=======================================================================
	mov	word[pos]	,0
	mov	ax, 0600h		
	mov	bx, 0700h		
	mov	cx, 0			
	mov	dx, 0184fh		
	int	10h			
	ret

;=======================================================================
disp_str:
;=======================================================================
	push	eax
	push	ebx
	push	ecx
	push	edx

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]

	mov	edx,eax
	mov	cl,0x10
	shr	edx,cl
	mov	cl, 0x10
.1:	
	div	cl
	cmp	al, 0
	je	.3
	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1

;a~f
.2:
	add	al, 0x57
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1
.3:
	cmp	ah, 9
	ja	.4

	add	ah, 0x30
	mov	byte[es:bx],ah
	add	bx, 2
	jmp	.5
.4:
	add	ah, 0x57
	mov	byte[es:bx],ah
	add	bx, 2
.5:
	mov	[pos], bx

	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
		
	ret

;=======================================================================
disp_num16:
; eax is parameter which is the value need to be displayed
;=======================================================================
	push	eax
	push	ebx
	push	ecx

	mov	ebx, eax
	and	ebx, 0xF0000000
	mov	cl, 28
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x0F000000
	mov	cl, 24
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x00F00000
	mov	cl, 20
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x000F0000
	mov	cl, 16
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x0000F000
	mov	cl, 12
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x00000F00
	mov	cl, 8
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x000000F0
	mov	cl, 4
	shr	ebx, cl	
	call	disp_num16_2

	mov	ebx, eax
	and	ebx, 0x0000000F
	call	disp_num16_2

	pop	ecx
	pop	ebx
	pop	eax	

	ret

;=======================================================================
disp_num16_2:
; ebx is parameter which is the value need to be displayed
;=======================================================================

	push	eax
	push	esi	

	mov	si, word[pos]

	mov	eax, ebx

	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	ah, 0x7					; for disp

	mov	[es:si], ax
	add	si, 2
	
	jmp	.end

;a~f
.2:
	add	al, 0x57
	mov	ah, 0x7					; for disp

	mov	[es:si], ax
	add	si, 2

.end:
	mov	word[pos], si

	pop	esi
	pop	eax

	ret

;=====================
wait_1000:
;(1000*1000)
;=====================
	push	ecx
	push	edx

	xor	ecx, ecx

.0:
	xor 	edx, edx
.1:	
	nop
	
	add	edx, 1
	cmp	edx, 0x1000	;1024
	jne	.1
	
	add	ecx, 1
	cmp	ecx, 0x1000	;1024
	jne	.0

	pop	edx
	pop	ecx

	ret
