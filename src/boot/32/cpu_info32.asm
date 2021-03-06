;===========================================================================
; cpu_info32.asm 
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

;===============================================================
cpu_info32:
;===============================================================

	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	call	disp_cpu_heading

;------------------------------------------------------------
; display cpu type(Intel or Amd)
;------------------------------------------------------------
	xor	eax, eax
	
	cpuid

	mov	ebp, ebx
	call	disp_cpu_type

	mov	ebp, edx
	call	disp_cpu_type

	mov	ebp, ecx
	call	disp_cpu_type

;------------------------------------------------------------
; display cpu n-86
;------------------------------------------------------------

	mov	eax, 1
	
	cpuid

	mov	ebp, eax
	and	ebp, 0xF00			; bit 8 ~ 11 of eax
	mov	cl, 8
	shr	ebp, cl

	call	disp_cpu_n86


.end:

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp

	ret

;===============================================================
disp_cpu_type:
; ebp is parameter
;===============================================================
	
	push	ecx
	push 	ebx
	push	eax

;1)
	mov	ebx, ebp
	
	mov	al, bl
	mov	ah, 0x7				; fore-color is white
	call	disp_sym32			; 

;2)
	mov	ebx, ebp
	and	ebx, 0x0000FF00
	mov	cl, 8
	shr	ebx, cl

	mov	al, bl
	mov	ah, 0x7				; fore-color is white
	call	disp_sym32			; 

;3)
	mov	ebx, ebp
	and	ebx, 0x00FF0000
	mov	cl, 16
	shr	ebx, cl

	mov	al, bl
	mov	ah, 0x7				; fore-color is white
	call	disp_sym32			; 


;4)
	mov	ebx, ebp
	and	ebx, 0xFF000000
	mov	cl, 24
	shr	ebx, cl

	mov	al, bl
	mov	ah, 0x7				; fore-color is white
	call	disp_sym32			; 


	pop	eax
	pop	ebx
	pop	ecx
	
	ret


;===============================================================
disp_cpu_n86:
; ebp is parameter
;===============================================================
	
	push	eax

	
	cmp	ebp, 3
	je	.cpu386

	cmp	ebp, 4
	je	.cpu486

	cmp	ebp, 5
	je	.cpuPentium

	cmp	ebp, 6
	je	.cpuPentiumPro

	cmp	ebp, 2
	je	.cpuDualProcessors


.cpu386:

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '3'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '8'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '6'
	call	disp_sym32			; 


	jmp	.end
	
.cpu486:

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '4'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '8'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '6'
	call	disp_sym32			; 

	jmp	.end

.cpuPentium:

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'P'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 't'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'm'
	call	disp_sym32			; 

	jmp	.end

.cpuPentiumPro:

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'P'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 't'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'm'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'I'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'I'
	call	disp_sym32			; 


	jmp	.end

.cpuDualProcessors:	

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'D'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'l'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'c'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'p'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	
	jmp	.end


.end:
	pop	eax
	
	
	ret


;===============================================================
disp_cpu_heading:
;===============================================================

	push	eax

	
	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'C'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'P'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'U'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

