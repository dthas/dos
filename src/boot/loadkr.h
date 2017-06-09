;===========================================================================
; loadkr.h
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


	LOAD_ADDR	equ	0x9000

	KERNEL_BIN_SEG	equ	0x1200
	KERNEL_BIN_ADDR	equ	0x12000

	KERNEL_ADDR	equ	0x100000
	KERNEL_ADDR_V	equ	0xC0000000	; 3G

	TEMP_SEG_1RD	equ	0x4200		; just for the function of read_1lrd, which read a 1 level block in memory
	TEMP_SEG_2RD	equ	0x5200		; just for the function of read_2lrd, which read a 2 level block in memory

