;===========================================================================
; idt.asm 
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

[BITS	32]

%include	"idt.h"

extern	syscall
extern	sys_enpage
extern	page_driver

global	idt_divide_handler
global	idt_debug_handler
global	idt_n_mask_handler
global	idt_breakpoint_handler
global	idt_overflow_handler
global	idt_bound_handler
global	idt_invalid_handler
global	idt_double_fault_handler
global	idt_process_bound_handler
global	idt_tss_handler
global	idt_segment_handler
global	idt_stack_handler
global	idt_protect_handler
global	idt_page_handler
global	idt_intel_preseve_handler
global	idt_fpu_handler
global	idt_alignment_handler
global	idt_machine_check_handler
global	idt_simd_handler
global	idt_irq0_handler
global	idt_irq1_handler
global	idt_irq2_handler
global	idt_irq3_handler
global	idt_irq4_handler
global	idt_irq5_handler
global	idt_irq6_handler
global	idt_irq7_handler
global	idt_irq8_handler
global	idt_irq9_handler
global	idt_irq10_handler
global	idt_irq11_handler
global	idt_irq14_handler
global	idt_irq15_handler
;===============================================================
init_idt:
;===============================================================

	call	idt_setting

	ret

;===============================================================
idt_setting:
;===============================================================
	push	ebp
	push	edi
	push 	ebx
	push	eax

;------------------------------------------------------------
;eax: base addr; ebx: privilege; edi: type; ebp: IDT index
;------------------------------------------------------------
	;Init IDT_DIVIDE
	mov	ebp, IDT_DIVIDE
	mov	eax, idt_divide_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_DEBUG
	mov	ebp, IDT_DEBUG
	mov	eax, idt_debug_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_N_MASK
	mov	ebp, IDT_N_MASK
	mov	eax, idt_n_mask_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_BREAKPOINT
	mov	ebp, IDT_BREAKPOINT
	mov	eax, idt_breakpoint_handler
	mov	ebx, PRIV_USER
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_OVERFLOW
	mov	ebp, IDT_OVERFLOW
	mov	eax, idt_overflow_handler
	mov	ebx, PRIV_USER
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_BOUND
	mov	ebp, IDT_BOUND
	mov	eax, idt_bound_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_INVALID
	mov	ebp, IDT_INVALID
	mov	eax, idt_invalid_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_N_EQUT
	mov	ebp, IDT_N_EQUT
	mov	eax, idt_n_equt_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_DOUBLE_FAULT
	mov	ebp, IDT_DOUBLE_FAULT
	mov	eax, idt_double_fault_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_PROCESS_BOUND
	mov	ebp, IDT_PROCESS_BOUND
	mov	eax, idt_process_bound_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_TSS
	mov	ebp, IDT_TSS
	mov	eax, idt_tss_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_SEGMENT
	mov	ebp, IDT_SEGMENT
	mov	eax, idt_segment_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_STACK
	mov	ebp, IDT_STACK
	mov	eax, idt_stack_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_PROTECT
	mov	ebp, IDT_PROTECT
	mov	eax, idt_protect_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_PAGE
	mov	ebp, IDT_PAGE
	mov	eax, idt_page_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_INTEL_PRESEVE
	mov	ebp, IDT_INTEL_PRESEVE
	mov	eax, idt_intel_preseve_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_FPU
	mov	ebp, IDT_FPU
	mov	eax, idt_fpu_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_ALIGNMENT
	mov	ebp, IDT_ALIGNMENT
	mov	eax, idt_alignment_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_MACHINE_CHECK
	mov	ebp, IDT_MACHINE_CHECK
	mov	eax, idt_machine_check_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_SIMD
	mov	ebp, IDT_SIMD
	mov	eax, idt_simd_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting
			
	;Init IDT_IRQ0
	mov	ebp, IDT_IRQ0
	mov	eax, idt_irq0_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ1
	mov	ebp, IDT_IRQ1
	mov	eax, idt_irq1_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting
	
	;Init IDT_IRQ2
	mov	ebp, IDT_IRQ2
	mov	eax, idt_irq2_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ3
	mov	ebp, IDT_IRQ3
	mov	eax, idt_irq3_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ4
	mov	ebp, IDT_IRQ4
	mov	eax, idt_irq4_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ5
	mov	ebp, IDT_IRQ5
	mov	eax, idt_irq5_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting		
	
	;Init IDT_IRQ6
	mov	ebp, IDT_IRQ6
	mov	eax, idt_irq6_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ7
	mov	ebp, IDT_IRQ7
	mov	eax, idt_irq7_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ8
	mov	ebp, IDT_IRQ8
	mov	eax, idt_irq8_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ9
	mov	ebp, IDT_IRQ9
	mov	eax, idt_irq9_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ10
	mov	ebp, IDT_IRQ10
	mov	eax, idt_irq10_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ11
	mov	ebp, IDT_IRQ11
	mov	eax, idt_irq11_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ12
	mov	ebp, IDT_IRQ12
	mov	eax, idt_irq12_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ13
	mov	ebp, IDT_IRQ13
	mov	eax, idt_irq13_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ14
	mov	ebp, IDT_IRQ14
	mov	eax, idt_irq14_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init IDT_IRQ15
	mov	ebp, IDT_IRQ15
	mov	eax, idt_irq15_handler
	mov	ebx, PRIV_KERNEL
	mov	edi, ATTR_IGATE
	call	gate_setting

	;Init SYSCALL
	mov	ebp, INT_SYSCALL
	mov	eax, syscall_
	mov	ebx, PRIV_USER
	mov	edi, ATTR_IGATE
	call	gate_setting

	
	pop	eax
	pop	ebx
	pop	edi
	pop	ebp

	ret

;===============================================================
gate_setting
;eax: base addr; ebx: privilege; edi: type; ebp: IDT index
;===============================================================

	push	ecx
	push 	edx
	
	mov	cl, 3
	shl	ebp, cl			; ebp = ebp * 8
	add	ebp, idt_desc

	mov	edx, eax
	mov	word[ds:ebp + 0], dx	; 0, 1:low offset

	mov	dx, SEL_CODE
	mov	word[ds:ebp + 2], dx	; 2, 3:selector

	mov	dl, 0
	mov	byte[ds:ebp + 4], dl	; 4: attr1

	mov	edx, ebx
	mov	cl, 5
	shl	dx, cl			; privilege
	or	dx, di			; type | privilege
	mov	byte[ds:ebp + 5], dl	; 5: attr2
	
	mov	edx, eax
	mov	cl, 16
	shr	edx, cl	
	mov	word[ds:ebp + 6], dx	; 6, 7:high offset

			
	pop 	edx
	pop	ecx

	ret


;===============================================================
idt_divide_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg0
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt

;===============================================================
idt_debug_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg1
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt

;===============================================================
idt_n_mask_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg2
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt
;===============================================================
idt_breakpoint_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg3
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt
;===============================================================
idt_overflow_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg4
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt
;===============================================================
idt_bound_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg5
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt

;===============================================================
idt_invalid_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg6
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt

;===============================================================
idt_n_equt_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg7
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt

;===============================================================
idt_double_fault_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg8
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt	
	
;===============================================================
idt_process_bound_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg9
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt	
	
;===============================================================
idt_tss_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg10
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt	
;===============================================================
idt_segment_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg11
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt	 
;===============================================================
idt_stack_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg12
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt		
;===============================================================
idt_protect_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg13
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt		 
	 
;===============================================================
idt_page_handler:
;===============================================================

	add	esp, 4			; ignore error code, and then esp point to eip

	call	save

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)

	sti

	mov	eax, cr2		; the linear address of the page that not in memory are saved in cr2
	push	eax
	call	page_driver
	pop	eax

	cli

	jmp	restore_rein

	 
;===============================================================
idt_intel_preseve_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg15
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt		 
;===============================================================
idt_fpu_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg16
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt	 
;===============================================================
idt_alignment_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg17
	push	eax
	call	disp_str
	pop	eax

	add	esp, 8			; has error code, esp=>EIP
	hlt	 
;===============================================================
idt_machine_check_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg18
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt	 
	 
;===============================================================
idt_simd_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg19
	push	eax
	call	disp_str
	pop	eax

	add	esp, 4			; no error code, esp=>EIP
	hlt	
	 

;===============================================================
idt_irq0_handler:
;===============================================================

	call	save

	add	dword[flg_int_rein], 1
	cmp	dword[flg_int_rein], 0
	jne	.rein

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)

	push	IDT_IRQ0
	call	dis_irq
	pop	eax

	mov	al, EOI
	out	PRIMARY_PORT_20, al

	call	clock_driver

	push	IDT_IRQ0
	call	en_irq	
	pop	eax

	sub	dword[flg_int_rein], 1
	jmp	restore

.rein:
	sub	dword[flg_int_rein], 1
	jmp	restore_rein
	
;===============================================================
idt_irq1_handler:
;===============================================================

	call	save

	add	dword[flg_int_rein], 1
	cmp	dword[flg_int_rein], 0
	jne	.rein

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)

	push	IDT_IRQ1				; in case of reenter, so need to disable the interrupt
	call	dis_irq
	pop	eax

	mov	al, EOI
	out	PRIMARY_PORT_20, al

	call	keyb_handler

	push	IDT_IRQ1				; after handling the interrupt function, enable the interrupt
	call	en_irq
	pop	eax	

	sub	dword[flg_int_rein], 1
	jmp	restore

.rein:
	sub	dword[flg_int_rein], 1

	jmp	restore_rein
	

;===============================================================
idt_irq2_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg32
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq3_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg33
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq4_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg34
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq5_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg35
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq6_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg36
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq7_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg37
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq8_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg38
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq9_handler:
;===============================================================

	call	save

	add	dword[flg_int_rein], 1
	cmp	dword[flg_int_rein], 0
	jne	.rein

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)

	push	IDT_IRQ9
	call	dis_irq
	pop	eax

	mov	al, EOI
	out	PRIMARY_PORT_20, al
	
	out	SLAVE_PORT_A0, al

	call	rtl8029_handler	

	push	IDT_IRQ9
	call	en_irq	
	pop	eax


	sub	dword[flg_int_rein], 1
	jmp	restore

.rein:
	sub	dword[flg_int_rein], 1
	jmp	restore_rein

;===============================================================
idt_irq10_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg40
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq11_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg41
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq12_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg42
	push	eax
	call	disp_str
	pop	eax

	ret
;===============================================================
idt_irq13_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg43
	push	eax
	call	disp_str
	pop	eax

	ret

;===============================================================
idt_irq14_handler:
;===============================================================

	call	save

	add	dword[flg_int_rein], 1
	cmp	dword[flg_int_rein], 0
	jne	.rein

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)


	push	IDT_IRQ14
	call	dis_irq
	pop	eax

	mov	al, EOI
	out	PRIMARY_PORT_20, al
	
	out	SLAVE_PORT_A0, al

	call	harddisk_driver	

	push	IDT_IRQ14
	call	en_irq	
	pop	eax

	sub	dword[flg_int_rein], 1
	jmp	restore

.rein:
	sub	dword[flg_int_rein], 1
	jmp	restore_rein
	

;===============================================================
idt_irq15_handler:
;===============================================================
	xor	eax, eax
	mov	eax, mesg45
	push	eax
	call	disp_str
	pop	eax

	ret


;===============================================================
idt_desc_setting:
;===============================================================

	push	eax	

	mov	ax, 2047			; 2048 - 1
	mov	word[idtr], ax			; limit = 2047

	mov	eax, idt_desc
	mov	dword[idtr+2], eax		; base addr = the start address of GDT address within idtr

	lidt	[idtr]

	pop	eax

	ret


;===============================================================
idt_handling:
;===============================================================

	mov	ebx, [esp + 4]
	
	push	ebx
	call	dis_irq
	pop	ebx

	mov	al, EOI
	out	PRIMARY_PORT_20, al

	sti

	call	keyb_handler

	cli

	push	ebx
	call	en_irq
	pop	ebx	

	ret


	

