;===========================================================================
; hd_info.h
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

;=======================================================
;hard disk parameters
;=======================================================
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;command block registers
;------------------------------------------------------
cmd32:		db	0	;data
		db	0	;err/feature
		db	0	;sector count
		db	0	;lba low
		db	0	;lba mid
		db	0	;lba high
		db	0	;device
		db	0	;status/command
;------------------------------------------------------

;command block ports(primary)
;------------------------------------------------------
CBP_PORT0_	equ	0x1f0		;data
CBP_PORT1_	equ	0x1f1		;err/feature
CBP_PORT2_	equ	0x1f2		;sector count
CBP_PORT3_	equ	0x1f3		;lba low
CBP_PORT4_	equ	0x1f4		;lba mid
CBP_PORT5_	equ	0x1f5		;lba high
CBP_PORT6_	equ	0x1f6		;device/command
CBP_PORT7_	equ	0x1f7		;status
;------------------------------------------------------

;control block ports
;------------------------------------------------------
CBTP_PORT0_	equ	0X3F6		;alternate/device,status/control
;------------------------------------------------------

;IDENTITY returns : hardware parameters
;------------------------------------------------------
NUM_SEQ_START	equ	20	;10
NUM_SEQ_END	equ	40	;19

TYPE_START	equ	54	;27
TYPE_END	equ	94	;46

NUM_SEC_START	equ	120	;60
NUM_SEC_END	equ	124	;61

LBA_SUP_START	equ	98	;49
LBA_SUP_END	equ	102	;50

INS_SET_START	equ	166	;83
INS_SET_END	equ	170	;84
;------------------------------------------------------

HD_IDEN		equ	0xEC
HD_WRITE	equ	0x30

