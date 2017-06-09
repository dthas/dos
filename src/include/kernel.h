//===========================================================================
// kernel.h
//   Copyright (C) 2012 Free Software Foundation, Inc.
//   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
//
//This file is part of DTHAS.
//
//DTHAS is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or 
//(at your option) any later version.
//
//DTHAS is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
//===========================================================================

#ifndef	_X_KERNEL_H_
#define	_X_KERNEL_H_

//-------------------------------------------------------------------------
// 8259A
//-------------------------------------------------------------------------
#define	PRIMARY_PORT_20	0x20		
#define	PRIMARY_PORT_21	0x21
#define	SLAVE_PORT_A0	0xA0
#define	SLAVE_PORT_A1	0xA1

//-------------------------------------------------------------------------
// IRQ(in vector table)
//-------------------------------------------------------------------------
#define	INT_IRQ0	0x20
#define	INT_IRQ1	0x21
#define	INT_IRQ2	0x22
#define	INT_IRQ3	0x23
#define	INT_IRQ4	0x24
#define	INT_IRQ5	0x25
#define	INT_IRQ6	0x26
#define	INT_IRQ7	0x27
#define	INT_IRQ8	0x28
#define	INT_IRQ9	0x29
#define	INT_IRQ10	0x2A
#define	INT_IRQ11	0x2B
#define	INT_IRQ12	0x2C
#define	INT_IRQ13	0x2D
#define	INT_IRQ14	0x2E
#define	INT_IRQ15	0x2F

#define	NUM_IRQ		16

//-------------------------------------------------------------------------
// IRQ(in 8259)
//-------------------------------------------------------------------------
#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CONCATE_IRQ	2
#define	HARDDISK_IRQ	14

//-------------------------------------------------------------------------
// GDT
//-------------------------------------------------------------------------
#define	DESC_SIZE	0x8		// 8 bytes per descriptor

#define	IDX_HEADER	0
#define	IDX_CODE	1
#define	IDX_DATA	2
#define	IDX_DISP	3
#define	IDX_TSS		4
#define	IDX_PROC	5

#define	SEL_HEADER	0
#define	SEL_CODE	0x8
#define	SEL_DATA	0x10
#define	SEL_DISP	(0x18+3)
#define	SEL_TSS		0X20
#define	SEL_RPOC_FIRST	0x28

#define	ATTR_CODE_READ_EXE	0x9A
#define	ATTR_CODE_32		0x4000
#define	ATTR_UNIT_4K		0x8000
//-------------------------------------------------------------------------
// CLOCK
//-------------------------------------------------------------------------
#define	TIME_43		0x43
#define	TIME_40		0x40
#define TIME_FREQUENCE	0x1234DE
#define TIME_HZ		0x64
#define TIME_RATE_GEN	0x34

//-------------------------------------------------------------------------
// IDT reenter
//-------------------------------------------------------------------------
#define	INT_REENTER_L		-1
#define	INT_REENTER_H		0
		
PUBLIC	u32	wait_schd_tick;
struct proc *	proc_schd_next;

#endif
