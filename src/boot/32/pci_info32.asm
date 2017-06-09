;===========================================================================
; pci_info32.asm 
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

%include	"32/pci.h"
;===============================================================
pci_info32:
; bus		: 0 ~ 4
; device	: 0 ~ 31
; function	: 0 ~ 7
; Traverse each bus, device and function. list all their info
;===============================================================
	push	edi	
	push	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	call	disp_pci_heading

;------------------------------------------------------------
; display PCI device information
;------------------------------------------------------------
	mov	ch, 11

	xor	ebx, ebx
	xor	esi, esi
	
.0:
	xor	ebp, ebp
.10:
	xor	edi, edi
.20:
	mov	eax, BASE_ADDR
	
	mov	esi, ebx
	mov	cl, 16
	shl	esi, cl				; ebx = bus * 0x10000
	add	eax, esi

	mov	esi, ebp
	mov	cl, 8
	shl	esi, cl				; ebp = device * 0x100 
	add	eax, esi

	mov	esi, edi
	mov	cl, 3
	shl	esi, cl				; ebp = function * 8 
	add	eax, esi

	mov	dword[bus_dev_fun_addr], eax
	
	mov	dx, CONF_PORT
	out	dx, eax

	mov	dx, DATA_PORT
	in	eax, dx

	cmp	eax, 0xFFFFFFFF
	je	.100

	cmp	eax, 0
	je	.100

	mov	esi, eax

;------------------------------------------------------------
; update pos32
;------------------------------------------------------------
	mov	eax, 160
	mul	ch
	mov	word[pos32], ax
	
	add	ch, 1
;------------------------------------------------------------
; display bus
;------------------------------------------------------------


;------------------------------------------------------------
; display device
;------------------------------------------------------------
	mov	eax, ebp
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;------------------------------------------------------------
; display function
;------------------------------------------------------------
	mov	eax, edi
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;------------------------------------------------------------
; display vendor id
;------------------------------------------------------------
	mov	eax, esi
	and	eax, 0x0000FFFF
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;
		
;------------------------------------------------------------
; display device id
;------------------------------------------------------------
	mov	eax, esi
	and	eax, 0xFFFF0000
	mov 	cl, 16
	shr	eax, cl
	
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;------------------------------------------------------------
; display class
;------------------------------------------------------------

	mov	eax, dword[bus_dev_fun_addr]
	or	eax, 0x8

	mov	dx, CONF_PORT
	out	dx, eax

	mov	dx, DATA_PORT
	in	eax, dx

	cmp	eax, 0xFFFFFFFF
	je	.100

	mov	esi, eax

	;display irq line(lowest 8 bit)
	and	eax, 0xFFFFFF00
	mov	cl, 8
	shr	eax, cl
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;------------------------------------------------------------
; display io-base(base address 0)
;------------------------------------------------------------

	mov	eax, dword[bus_dev_fun_addr]
	or	eax, 0x10

	mov	dx, CONF_PORT
	out	dx, eax

	mov	dx, DATA_PORT
	in	eax, dx

	cmp	eax, 0xFFFFFFFF
	je	.100

	mov	esi, eax

	;display irq line(lowest 8 bit)
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;------------------------------------------------------------
; display irq
;------------------------------------------------------------

	mov	eax, dword[bus_dev_fun_addr]
	or	eax, 0x3C

	mov	dx, CONF_PORT
	out	dx, eax

	mov	dx, DATA_PORT
	in	eax, dx

	cmp	eax, 0xFFFFFFFF
	je	.100

	mov	esi, eax

	;display irq line(lowest 8 bit)
	and	eax, 0xFF
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;



.100:
	add	edi, 1				; function
	cmp	edi, FUN_MAX			; 8
	jb	.20

	add	ebp, 1				; dev
	cmp	ebp, DEV_MAX			; 32
	jb	.10

	add	ebx, 1				; bus
	cmp	ebx, BUS_MAX			; 5
	jb	.0

.end:

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop	esi
	pop	edi

	ret


;===============================================================
disp_pci_heading:
;===============================================================
	;call	disp_bus_heading

	call	disp_dev_heading

	call	disp_function_heading

	call	disp_vendor_heading

	call	disp_device_heading

	call	disp_class_heading

	call	disp_iobase_heading

	call	disp_irq_heading
		
	ret


;===============================================================
disp_device_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'D'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'v'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'c'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret


;===============================================================
disp_vendor_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'V'
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
	mov	al, 'd'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'o'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'r'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret



;===============================================================
disp_bus_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'B'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_dev_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'D'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'v'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret
;===============================================================
disp_function_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'F'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'u'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'c'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret


;===============================================================
disp_class_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'C'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'l'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_irq_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'I'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'R'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'Q'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret


;===============================================================
disp_iobase_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'I'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'O'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '-'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'B'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret


;-----------------------------------------------------------------------
bus_dev_fun_addr	dd	0		; used in pci_info
;=======================================================================
