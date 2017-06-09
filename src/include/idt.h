;===========================================================================
; idt.h
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

KERNEL_PAGE_BASE	equ	0x200000

;-------------------------------------------------------------------------
; structure
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; IDT index(exception)
;-------------------------------------------------------------------------
	IDT_DIVIDE		equ	0
	IDT_DEBUG		equ	1
	IDT_N_MASK		equ	2
	IDT_BREAKPOINT		equ	3
	IDT_OVERFLOW		equ	4
	IDT_BOUND		equ	5
	IDT_INVALID		equ	6
	IDT_N_EQUT		equ	7
	IDT_DOUBLE_FAULT	equ	8
	IDT_PROCESS_BOUND	equ	9
	IDT_TSS			equ	10
	IDT_SEGMENT		equ	11
	IDT_STACK		equ	12
	IDT_PROTECT		equ	13
	IDT_PAGE		equ	14
	IDT_INTEL_PRESEVE	equ	15
	IDT_FPU			equ	16
	IDT_ALIGNMENT		equ	17
	IDT_MACHINE_CHECK	equ	18
	IDT_SIMD		equ	19

;-------------------------------------------------------------------------
; IDT index(interrupt)
;-------------------------------------------------------------------------
	IDT_IRQ0		equ	0x20
	IDT_IRQ1		equ	0x21
	IDT_IRQ2		equ	0x22
	IDT_IRQ3		equ	0x23
	IDT_IRQ4		equ	0x24
	IDT_IRQ5		equ	0x25
	IDT_IRQ6		equ	0x26
	IDT_IRQ7		equ	0x27
	IDT_IRQ8		equ	0x28
	IDT_IRQ9		equ	0x29
	IDT_IRQ10		equ	0x2A
	IDT_IRQ11		equ	0x2B
	IDT_IRQ12		equ	0x2C
	IDT_IRQ13		equ	0x2D
	IDT_IRQ14		equ	0x2E
	IDT_IRQ15		equ	0x2F
	
;-------------------------------------------------------------------------
; SYSCALL index()
;-------------------------------------------------------------------------
	INT_SYSCALL		equ	0x81

;-------------------------------------------------------------------------
; 8259A
;-------------------------------------------------------------------------
	I_PRIMARY_PORT_20		equ	0x20		
	I_PRIMARY_PORT_21		equ	0x21
	I_SLAVE_PORT_A0		equ	0xA0
	I_SLAVE_PORT_A1		equ	0xA1

;-------------------------------------------------------------------------
; IDT reenter
;-------------------------------------------------------------------------
	INT_REENTER_L		equ	-1
	INT_REENTER_H		equ	0
