;===========================================================================
; krl.h
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

	DESC_SYS_FIRST		equ	0x28

;-------------------------------------------------------------------------
; PRIVILEGE
;-------------------------------------------------------------------------
	PRIV_KERNEL		equ	0
	PRIV_USER		equ	3
	

;-------------------------------------------------------------------------
; TYPE
;-------------------------------------------------------------------------
	ATTR_IGATE		equ	0x8E

;-------------------------------------------------------------------------
; GDT
;-------------------------------------------------------------------------
	SEL_CODE		equ	0x8
	SEL_DATA		equ	0x10


;-------------------------------------------------------------------------
; 8259A
;-------------------------------------------------------------------------
	PRIMARY_PORT_20		equ	0x20
	PRIMARY_PORT_21		equ	0x21
	SLAVE_PORT_A0		equ	0xA0
	SLAVE_PORT_A1		equ	0xA1

	EOI			equ	0x20

;-------------------------------------------------------------------------
; TSS
;-------------------------------------------------------------------------

	ATTR_TSS		equ	0x89

	SEL_TSS			equ	0x20
	GDT_TSS			equ	0x20
	
;point to the space next to ss reg
	TSS_ESP0		equ	68

;-------------------------------------------------------------------------
; proc table
;-------------------------------------------------------------------------
;   sizeof(struct registers) + LDT_NUM * sizeof(struct p_desc) + FILE_NUM *sizeof(struct f_desc)
; = 16*4 + 3*2*4 + 20*4*4 = 408 
; 408 - 4 = 404(offset from struct proc, should be from 0)
	LDT_SEL			equ	68
	REG_TEMP		equ	44	




;-------------------------------------------------------------------------
; page setting
;-------------------------------------------------------------------------
	PG_N_EXIST		equ	0	; page not exist bit
	PG_EXIST		equ	1	; page exist bit
	PG_RW_W			equ	2	; read/write bit
	PG_US_SYS		equ	0	; user/system bit, this is for system
	PG_US_USER		equ	4	; user/system bit, this is for user
	PG_PG			equ	0x100	; Global page, which uses for unchange 3G ~ 4G page while cr3 flushes within switching

	KNL_PAGE_DIR_BASE	equ	0x200000	;2MB
	KNL_PAGE_TABLE_BASE	equ	0x201000	;2.004MB

	USER_TABLE_PAGE_COUNT	equ	0xC0000
	KERNEL_TABLE_PAGE_COUNT	equ	0x40000

	KERNEL_ADDR		equ	0x100000

	PG_DIR			equ	72	;offset in "struct proc"


