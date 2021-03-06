;===========================================================================
; lib.asm
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
;========
disp_msg:
;========
	mov	bp, ax
	mov	ax, 01301h
	mov	bx, 0007h
	mov	dl, 0
	int	10h
	
	ret

;========
disp_str:
;========
	push	eax
	push	ebx
	push	ecx
	push	edx

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]

	mov	edx,eax
	mov	cl,0x10
	shr	edx,cl
	mov	cl, 0x10
.1:	
	div	cl
	cmp	al, 0
	je	.3
	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1

;a~f
.2:
	add	al, 0x57
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1
.3:
	cmp	ah, 9
	ja	.4

	add	ah, 0x30
	mov	byte[es:bx],ah
	add	bx, 2
	jmp	.5
.4:
	add	ah, 0x57
	mov	byte[es:bx],ah
	add	bx, 2
.5:
	mov	[pos], bx

	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
		
	ret

;========
clr_sc:
;========
	mov	word[pos]	,0
	mov	ax, 0600h		; AH = 6,  AL = 0
	mov	bx, 0700h		; (BL = 0x7)
	mov	cx, 0			; (0, 0)
	mov	dx, 0184fh		; (80, 50)
	int	10h			; int 0x10
	ret

;=====================
out_8:
;(port:dx; value:al)
;=====================
	out	dx, al
	nop
	nop

	ret

;=====================
in_8:
;(port:dx; value:al)
;=====================
	in	al, dx
	nop
	nop

	ret

;=====================
in_16:
;(port:dx; value:al)
;=====================
	in	ax, dx
	nop
	nop

	ret

;=====================
out_16:
;(port:dx; value:al)
;=====================
	out	dx, ax
	nop
	nop

	ret

;=====================
wait_1000:
;(1000*1000)
;=====================
	push	ecx
	push	edx

	xor	ecx, ecx

.0:
	xor 	edx, edx
.1:	
	nop
	
	add	edx, 1
	cmp	edx, 0x10	;1024
	jne	.1
	
	add	ecx, 1
	cmp	ecx, 0x10	;1024
	jne	.0

	pop	edx
	pop	ecx

	ret

;=====================
wait_100000:
;(1000*1000)
;=====================

	ret

;=====================
cmd_send:
;=====================

;send write to control block register
	mov	al, 0
	mov	dx, CBTP_PORT0
	call	out_8

;send command block registers
	
	mov	al, byte[cmd+1]
	mov	dx, CBP_PORT1
	call	out_8

	mov	al, byte[cmd+2]
	mov	dx, CBP_PORT2
	call	out_8

	mov	al, byte[cmd+3]
	mov	dx, CBP_PORT3
	call	out_8

	mov	al, byte[cmd+4]
	mov	dx, CBP_PORT4
	call	out_8

	mov	al, byte[cmd+5]
	mov	dx, CBP_PORT5
	call	out_8

	mov	al, byte[cmd+6]
	mov	dx, CBP_PORT6
	call	out_8

	mov	al, byte[cmd+7]
	mov	dx, CBP_PORT7
	call	out_8

	ret
;========================
read_sector:
;
;parm: 
;eax : start sector number
;cl  : sectors to transfer
;dx  : segment
;========================
	mov   	bx, dx               ; Reading to segment 0x1200
   	mov   	es, bx
   	xor   	bx, bx                  ; Offset 0

	mov	byte [dap +  0], 0x10
	mov	byte [dap +  1], 0
	mov	byte [dap +  2], cl
	mov	byte [dap +  3], 0
	mov	word [dap +  4], 0
	mov	word [dap +  6], dx
	mov	dword[dap +  8], eax
	mov	dword[dap + 12], 0      	

	mov   	dl, 0e0h
	mov   	ah, 042h

      	mov	si, dap
      	int   	13h   

      	ret

;============
sys_restart:
;============
	in 	al, 0x92
	IODELAY

	or	al, 1
	out	0x92, al

	hlt

;============
sys_poweroff:
;============
	
	mov	ax, 0x5301
	xor	bx, bx
	int	0x15

	mov	ax, 0x530e
	xor	bx, bx
	mov	cx, 0x102
	int	0x15

	mov	ax, 0x5307
	mov	bx, 1
	mov	cx, 3
	int	0x15	

	hlt
