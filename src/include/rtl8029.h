//===========================================================================
// rtl8029.h
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
#ifndef	_X_RTL8029_H_
#define	_X_RTL8029_H_

#define RDMAPORT	0x10
#define RESET		0x18

//===========================================================================
// page0
//===========================================================================
//-------------------------------------------------------------------------
// [R]
//-------------------------------------------------------------------------
#define CR	0x0
#define CLDA0	0x1
#define CLDA1	0x2
#define BNRY	0x3
#define TSR	0x4
#define NCR	0x5
#define FIFO	0x6
#define ISR	0x7
#define CRDA0	0x8
#define CRDA1	0x9
#define Z8029ID0	0xA
#define Z8029ID1	0xB
#define RSR	0xC
#define CNTR0	0xD
#define CNTR1	0xE
#define CNTR2	0xF

//-------------------------------------------------------------------------
// [W]
//-------------------------------------------------------------------------
#define PSTART	0x1
#define PSTOP	0x2
#define TPSR	0x4
#define TBCR0	0x5
#define TBCR1	0x6
#define RSAR0	0x8
#define RSAR1	0x9
#define RBCR0	0xA
#define RBCR1	0xB
#define RCR	0xC
#define TCR	0xD
#define DCR	0xE
#define IMR	0xF

//===========================================================================
// page1
//===========================================================================
//-------------------------------------------------------------------------
// [R/W]
//-------------------------------------------------------------------------
#define PAR0	0x1
#define PAR1	0x2
#define PAR2	0x3
#define PAR3	0x4
#define PAR4	0x5
#define PAR5	0x6
#define CURR	0x7
#define MAR0	0x8
#define MAR1	0x9
#define MAR2	0xA
#define MAR3	0xB
#define MAR4	0xC
#define MAR5	0xD
#define MAR6	0xE
#define MAR7	0xF

//===========================================================================
// page2
//===========================================================================
//-------------------------------------------------------------------------
// [R]
//-------------------------------------------------------------------------

//===========================================================================
// page3
//===========================================================================
//-------------------------------------------------------------------------
// [R]
//-------------------------------------------------------------------------
#define Z9346CR	0x1
#define CONFIG0	0x3
#define CONFIG2	0x5
#define CONFIG3	0x6

#define Z8029ASID0	0xE
#define Z8029ASID1	0xF

//-------------------------------------------------------------------------
// [R]
//-------------------------------------------------------------------------
#define HLTCLK	0x9

//===========================================================================
// command
//===========================================================================
//netcard not running
#define PAGE0_N	0x21
#define PAGE1_N	0x61
#define PAGE2_N	0xA1
#define PAGE3_N	0xE1

//netcard running
#define PAGE0_Y	0x22
#define PAGE1_Y	0x62
#define PAGE2_Y	0xA2

#define Z8029ID0_INIT_VAL	0x0
#define Z8029ID1_INIT_VAL	0x0

#define DCR_INIT_VAL	0xC8

#define RBCR0_INIT_VAL	0x0
#define RBCR1_INIT_VAL	0x0

#define RCR_INIT_VAL		0xE0
#define RCR_INIT_NORMAL_VAL	0xCC

#define TCR_INIT_VAL		0xE2
#define TCR_INIT_NORMAL_VAL	0xE0

#define TPSR_INIT_VAL	0x40
#define CURR_INIT_VAL	0x4D
#define BNRY_INIT_VAL	0x4C
#define PSTART_INIT_VAL	0x4C
#define PSTOP_INIT_VAL	0x80
#define ISR_INIT_VAL	0xFF
#define IMR_INIT_VAL	0x0
#define PAR0_INIT_VAL	0xb0
#define PAR1_INIT_VAL	0xc4
#define PAR2_INIT_VAL	0x20
#define PAR3_INIT_VAL	0x00
#define PAR4_INIT_VAL	0x00
#define PAR5_INIT_VAL	0x00

#define MAR0_INIT_VAL	0x0
#define MAR1_INIT_VAL	0x41
#define MAR2_INIT_VAL	0x0
#define MAR3_INIT_VAL	0x80
#define MAR4_INIT_VAL	0x0
#define MAR5_INIT_VAL	0x0
#define MAR6_INIT_VAL	0x0
#define MAR7_INIT_VAL	0x0

#define CR_TXP		0x4
#define CR_START_CMD	0x2
#define CR_REMOTE_READ	0x8
#define CR_REMOTE_WRTE	0x10
#define CR_END_DMA	0x20

#define ISR_PRX		0x1
#define ISR_PTX		0x2
#define ISR_OVW		0x10
#define ISR_RDC		0x40
#define ISR_RESET	0x80

//===========================================================================
// variable
//===========================================================================

struct net_dev 
{
	struct hwaddr mac; // really we should not specify this, but we only deal in ethernet
	struct iaddr ip;
	struct iaddr netmask;
        u16 iobase;
        u8 irq;
};

struct net_dev *pnet_dev;

#endif
