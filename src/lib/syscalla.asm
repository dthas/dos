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

IDX_MSGSEND	equ	0
IDX_MSGFORD	equ	1
IDX_MSGRECV	equ	2
IDX_MSGPROC	equ	3
IDX_MSGPRINT	equ	4
IDX_MSGATTR	equ	5

INT_SYSCALL	equ	0x81

global	send
global	forward
global	receive
global	setstatus
global	prints
global	setattr

;===============================================================
send:
;===============================================================
	mov	eax, IDX_MSGSEND
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret

;===============================================================
forward:
;===============================================================
	mov	eax, IDX_MSGFORD
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret
	
;===============================================================
receive:
;===============================================================
	mov	eax, IDX_MSGRECV
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret

;===============================================================
setstatus:
;===============================================================
	mov	eax, IDX_MSGPROC
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret

;===============================================================
prints:
;===============================================================
	mov	eax, IDX_MSGPRINT
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret

;===============================================================
setattr:
;===============================================================
	mov	eax, IDX_MSGATTR
	mov	ebx, dword[esp + 4]	; 1th param

	int	INT_SYSCALL
	ret
