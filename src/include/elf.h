;===========================================================================
; elf.h
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

;==========================================================================
;ELF header
;==========================================================================
;-------------------------------------------------------------------------
elf_header:	dd	0	;0x0  ;(1) identify information(16 characters)
		dd	0	;
		dd	0	;
		dd	0	;
		dw	0	;0x10 ;(2) type
		dw	0	;0x12 ;(3) system architecture
		dd	0	;0x14 ;(4) version
		dd	0	;0x18 ;(5) program entry
		dd	0	;0x1c ;(6) program header table offset
		dd	0	;0x20 ;(7) section header table offset
		dd	0	;0x24 ;(8)
		dw	0	;0x28 ;(9) the size of ELF header(default is 0x34)
		dw	0	;0x2a ;(10)the size of program header table(default is 0x20)
		dw	0	;0x2c ;(11)how many entries in program header table
		dw	0	;0x2e ;(12)the size of section header table(default is 0x28)
		dw	0	;0x30 ;(13)how many entries in section header table
		dw	0	;0x32 ;(14)
;-------------------------------------------------------------------------

;==========================================================================
;program header table
;==========================================================================
;-------------------------------------------------------------------------
prog_header:	dd	0	;0x0  ;(1) type
		dd	0	;0x4  ;(2) the offset of the first byte in the section
		dd	0	;0x8  ;(3) the virtual address of the first byte in the section
		dd	0	;0xc  ;(4) the physical address of the first byte in the section
		dd	0	;0x10 ;(5) the length of the section in file
		dd	0	;0x14 ;(6) the length of the section in memory
		dd	0	;0x18 ;(7) flags
		dd	0	;0x1c ;(8) alignment	
;-------------------------------------------------------------------------

ELF_HEADER_SIZE		equ	0x34
PROG_HEADER_SIZE	equ	0x20
SEC_HEADER_SIZE		equ	0x28
