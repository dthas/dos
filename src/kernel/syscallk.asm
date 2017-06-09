;===========================================================================
; syscall.asm 
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

;===============================================================
syscall_:
;===============================================================

	call	save

	add	dword[flg_int_rein], 1	
	cmp	dword[flg_int_rein], 0
	
	jne	.rein

	mov	esp, stacktop		; esp -> kernel stack(previous is pointed to proc_table)

	push	ebx			; 1th param
	call	[syscall_tab + eax * 4]
	pop	ebx

	
	mov	ebp, [proc_ready]	; ebp -> proc_ready
	mov	[ebp + 0], eax		; eax = return value

	sub	dword[flg_int_rein], 1
	jmp	restore

.rein:
	sub	dword[flg_int_rein], 1
	jmp	restore_rein


