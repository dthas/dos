;===========================================================================
; gdt.h
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

;------------------------------------------------------
; GDT has 4 descriptors
;1)
	GDT_HEADER:	dd	0
			dd	0
;2)
	GDT_CODE:	dd	0
			dd	0
;3)
	GDT_DATA:	dd	0
			dd	0
;4)
	GDT_DISP:	dd	0
			dd	0

;Just a label
	GDT_END:

;------------------------------------------------------
; GDT also has 3 selectors
;1)
	SEL_CODE:	equ	GDT_CODE - GDT_HEADER
;2)
	SEL_DATA:	equ	GDT_DATA - GDT_HEADER
;3)
	SEL_DISP:	equ	GDT_DISP - GDT_HEADER

;------------------------------------------------------
; gdtr register

	gdtr:		dw	GDT_END - GDT_HEADER - 1
			dd	LOAD_ADDR + GDT_HEADER

;------------------------------------------------------
; Attribute
	ATTR_CODE_READ_EXE	equ	0x9A
	ATTR_DATA_READ_WRITE	equ	0x92

	ATTR_CODE_32		equ	0x4000
	ATTR_UNIT_4K		equ	0x8000

	ATTR_DPL0		equ	0x0
	ATTR_DPL1		equ	0x20
	ATTR_DPL2		equ	0x40
	ATTR_DPL3		equ	0x60
