;===========================================================================
; krl.asm 
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

;variable 
extern	disp_dword
extern	disp_num
extern	disp_str
extern	init_8259a
extern	mcopy
extern	en_irq
extern	dis_irq
extern	desc_setting
extern 	proc_ready
extern	syscall_tab
;extern	current_tty

;function
extern	keyb_handler
extern	init_keyb
extern  init_tty
extern	tty_main
extern	init_hd
extern	init_vfs
extern	init_hd_freelist
extern 	harddisk_driver
extern	init_part
extern	init_clock
extern	clock_driver
extern	init_proc
extern	init_syscall_tab
extern	init_mesg
extern	init_sys
;extern	init_test
extern	init_uproc
extern	init_shell
extern	init_net
extern 	rtl8029_handler
extern	init_ftp
extern	init_tcp_daemon
extern	init_tftp
extern	init_dns
extern	init_dhcp
extern	init_snmp
extern	init_http
extern	init_smtp
extern	init_pop3
extern	init_telnet

extern	vfs_test

[section .data]
;=======================================================================
pos			dd	1920
current_tty		dd	0
cur_pid			dd	0
flg_int_rein		dd	0

ticket			dd	0

mesg200			db	"enter into kernel",0
;=======================================================================

[section .gdt_data]
;=======================================================================
; gdtr register

gdtr:			dw	0
			dd	0
;=======================================================================



[section .idt_data]
;=======================================================================
mesg0			db	"#DE", 0
mesg1			db	"#DB", 0
mesg2			db	"#NMI", 0
mesg3			db	"#BP", 0
mesg4			db	"#OF", 0
mesg5			db	"#BR", 0
mesg6			db	"#UD", 0
mesg7			db	"#NM", 0
mesg8			db	"#DF", 0
mesg9			db	"#CPB", 0
mesg10			db	"#TSS", 0
mesg11			db	"#NP", 0
mesg12			db	"#SS", 0
mesg13			db	"#GP", 0
mesg14			db	"#PF", 0
mesg15			db	"#IP1", 0
mesg16			db	"#MF", 0
mesg17			db	"#AC", 0
mesg18			db	"#MC", 0
mesg19			db	"#XF", 0

mesg30			db	"#IRQ0", 0
mesg31			db	"#IRQ1", 0
mesg32			db	"#IRQ2", 0
mesg33			db	"#IRQ3", 0
mesg34			db	"#IRQ4", 0
mesg35			db	"#IRQ5", 0
mesg36			db	"#IRQ6", 0
mesg37			db	"#IRQ7", 0
mesg38			db	"#IRQ8", 0
mesg39			db	"#IRQ9", 0
mesg40			db	"#IRQ10", 0
mesg41			db	"#IRQ11", 0
mesg42			db	"#IRQ12", 0
mesg43			db	"#IRQ13", 0
mesg44			db	"#IRQ14", 0
mesg45			db	"#IRQ15", 0
;=======================================================================


[section .tss]
;=======================================================================
tss:
			dd	0	;link
			dd	0	;esp0
			dd	0	;ss0
			dd	0	;esp1
			dd	0	;ss1
			dd	0	;esp2
			dd	0	;ss2
			dd	0	;cr3
			dd	0	;eip
			dd	0	;eflags
			dd	0	;eax
			dd	0	;ecx
			dd	0	;edx
			dd	0	;ebx
			dd	0	;esp
			dd	0	;ebp
			dd	0	;esi
			dd	0	;edi
			dd	0	;es
			dd	0	;cs
			dd	0	;ss
			dd	0	;ds
			dd	0	;fs
			dd	0	;gs
			dd	0	;ldt
			dw	0	;trap
			dw	0	;iobase

tss_len		equ	$ - tss					
;=======================================================================



;---------------------------------------------------------------------
; idtr register
idtr:			dw	0
			dd	0
;=======================================================================
;=======================================================================
[section .bss]
gdt_desc		resb	1024				;128 * 8
idt_desc 		resb 	2048				;256 * 8

;stack			resb 	1024
stack			resb 	8192
stacktop:
;=======================================================================

[section .text]

global	pos
global	current_tty
global	cur_pid
global	gdt_desc
global	idt_desc
global	flg_int_rein
global	ticket
global	_start
global	restore
global	syscall_

global	idt_irq0_handler
global	idt_irq1_handler	
global	idt_invalid_handler
global	idt_setting
global	idt_desc_setting
global	gdt_desc_setting
global	gdtr
global	idtr


%include	"krl.h"

;===============================================================
_start:
;===============================================================

	mov	esp, stacktop

;------------------------------------------------------------
; initial variables
;------------------------------------------------------------
	mov	dword[current_tty], 0

	mov	edi, (80*23+0)*2
	mov	dword[pos], edi

	mov	dword[current_tty], 0
	
	mov	dword[cur_pid], 0

	mov	dword[flg_int_rein], -1
	
	mov	dword[ticket], 0

	call	init_idt

	call	init_8259a

	call	init_hd

	call	init_part

	call	init_mesg

	call	init_sys

	call	init_vfs

	call	init_uproc			

	call	init_net			

	call	init_ftp			

	call	init_tcp_daemon			

	call	init_tftp			

	call	init_dns			

	call	init_dhcp			

	call	init_snmp			

	call	init_http			

	call	init_smtp			

	call	init_pop3			

	call	init_telnet			
	

;------------------------------------------------------------
; copy all the GDT desc into the array of gdt
;------------------------------------------------------------
	call	gdt_desc_setting		; in gdt.asm

;------------------------------------------------------------
; copy all the IDT desc into the array of idt
;------------------------------------------------------------
	call	idt_desc_setting		; in idt.asm

;------------------------------------------------------------
; setup tss
;------------------------------------------------------------
	call	tss_desc_setting		; in tss.asm


	call	init_keyb


	call	init_tty

	call	init_proc

	call	init_clock

	jmp	restore
	
	jmp	$


;===============================================================
save:
;===============================================================
	
	push	es
	push	fs
	push	gs
	push	ds
	
	push	ebp
	push	edi
	push	esi
	push	edx
	push	ecx
	push	ebx
	push	eax

	;REG_TEMP is 44 and [REG_TEMP] saves the value of eip_of_save
	add	esp, REG_TEMP

;ss is tss.ss0, which has been set to SEL_DATA in tss.asm. 
;while occuring ring1->ring0, ss will be set to SEL_DATA by tss	
	
	mov	cx, SEL_DATA
	mov	ds, cx
	mov	es, cx
	mov	fs, cx

	ret
	
;===============================================================
restore:
;===============================================================

	mov	esp, [proc_ready]
	lea	eax, [esp + TSS_ESP0]
	mov	dword[tss + 4], eax


;===============================================================
restore_rein:
;===============================================================

	mov	ebp, [proc_ready]
	mov	eax, [ebp + PG_DIR]
	mov	cr3, eax

	mov	esp, [proc_ready]

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	esi
	pop	edi
	pop	ebp

	pop	ds
	pop	gs
	pop	fs
	pop	es

	;ignore reg of temp
	add	esp, 4
	
;the next is eip, cs ,eflags, esp, ss

	iret
	
;===============================================================
; support programs
;===============================================================
%include	"idt.asm"
%include	"gdt.asm"
%include	"tss.asm"
%include	"syscallk.asm"
