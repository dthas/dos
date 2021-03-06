;===========================================================================
; hw_info32.asm 
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

%include	"32/cpu_info32.asm"
%include	"32/pci_info32.asm"
%include	"32/hd_info32.asm"
;===============================================================
hw_info_main32:
;===============================================================

	;should before load_krl_12_to_10
	mov	word[pos32], 640			; from the 4th line, (80*4)*2
	call	mem_size_test32

	mov	word[pos32], 480			; from the 3th line, (80*3)*2
	call	cpu_info32

	mov	word[pos32], 960			; from the 6th line, (80*6)*2
	call	hd_info32

	mov	word[pos32], 1600			; from the 10th line, (80*10)*2
	call	pci_info32

	ret


