;===========================================================================
; page.h 
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


PAGE_DIR_KERNEL		equ	0x200000
PAGE_TABLE_KERNEL	equ	0x201000

PAGE_TABLE_KERNEL_S	equ	0x201000 ; for user space   ( 0 ~ 3G)
PAGE_TABLE_KERNEL_K	equ	0x501000 ; for kernel space (3G ~ 4G)

USER_TABLE_PAGE_COUNT	equ	0xC0000
KERNEL_TABLE_PAGE_COUNT	equ	0x40000

PG_EXIST		equ	1	; page exist bit
PG_RW_W			equ	2	; read/write bit
PG_US_SYS		equ	0	; user/system bit, this is for system
PG_US_USER		equ	4	; user/system bit, this is for user
PG_PG			equ	0x100	; Global page, which uses for unchange 3G ~ 4G page while cr3 flushes within switching
PG_PGE			equ	0x80	; in CR4
