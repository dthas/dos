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


;-------------------------------------------------------------------------
; structure
;-------------------------------------------------------------------------

	PROC_SYS_FIRST		equ	0x28
	PROC_SYS_END		equ	0x1C0

	PROC_USER_FIRST		equ	0x1C0
	PROC_USER_END		equ	0x400

	ATTR_CODE_EXE		equ	0x98
	ATTR_CODE_READ_EXE	equ	0x9A
	ATTR_DATA_READ_WRITE	equ	0x92
	ATTR_CODE_32		equ	0x4000
	ATTR_UNIT_4K		equ	0x8000

	ATTR_PRIV_SYS		equ	0x60		;DPL = 3
	ATTR_PRIV_USER		equ	0x60		;DPL = 3


	




