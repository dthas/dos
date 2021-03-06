//===========================================================================
// rtl8029.c
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

#include	"type.h"
#include	"stdio.h"
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"tty.h"
#include	"keyboard.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"pci.h"
#include	"ipv4.h"
#include	"icmp.h"
#include	"net.h"
#include	"frame.h"
#include	"rtl8029.h"
#include	"prototype.h"

PRIVATE	s8 	init_net_dev(struct net_dev *pnet_dev);
PRIVATE	void 	init_rtl8029_dev();
PRIVATE	void 	set_page(u32 page);

PRIVATE struct net_dev rtl8029;
struct s_package npkg;

//===========================================================================
// init tty
//===========================================================================
PUBLIC	void 	init_rtl8029()
{
	package	 	= &npkg;
 
	pnet_dev 	= &rtl8029;

	if(!init_net_dev(pnet_dev))
	{
		printk("can not init rtl8029 net dev");
		printk("\n");
		
		while(1)
		{};
	}

	init_net_dev(pnet_dev);

	init_rtl8029_dev();

	en_irq(pnet_dev->irq);

//------for test
	char *str = "-init 8029-";
	disp_str(str);		
}

//===========================================================================
// net main
//===========================================================================
PRIVATE	s8 	init_net_dev(struct net_dev *pnet_dev)
{
	struct pci_device *pdev = pcidev;

	s32 i;
	s32 total = PCI_BUS_MAX * PCI_DEVICE_MAX * PCI_FUNCTION_MAX;
	s8 flg = FALSE;
	for(i=0; i<total, pdev; i++, pdev++)
	{
		if((pdev->vendor_id == PCI_VENDOR_ID_8029) &&(pdev->device_id == PCI_DEVICE_ID_8029))
		{
			flg = TRUE;
			break;
		}
	}

	if(flg == TRUE)
	{
		pnet_dev->iobase	= pdev->iobase;
		pnet_dev->irq		= pdev->irq;

		return 1;
	}
	else
	{
		return 0;
	}		
}

//===========================================================================
// set_page(u32 page)
//===========================================================================
PRIVATE	void 	set_page(u32 page)
{
	out_8((pnet_dev->iobase + CR), page);
}

//===========================================================================
// rtl8029_handler()
//===========================================================================
PUBLIC	void 	rtl8029_handler()
{
}

//===========================================================================
// init_rtl8029_dev()
//===========================================================================
PRIVATE	void 	init_rtl8029_dev()
{
	u16 port;
	u8  val;
	s32 i;

	//-------------------------------------------------------------------------
	// hardware reset
	//-------------------------------------------------------------------------

	
	//-------------------------------------------------------------------------
	// software reset
	//-------------------------------------------------------------------------
	set_page(PAGE0_N);		//0x21

	port	= pnet_dev->iobase + ISR;
	for(i=0; i<0x1000; i++)
	{
		val	= in_8(port);	
		if((val & ISR_RESET) != 0)
		{
			break;
		}
	}
	
	//-------------------------------------------------------------------------
	// (PAGE0) regs setting
	//-------------------------------------------------------------------------
	set_page(PAGE0_N);

	// RBCR0
	port	= pnet_dev->iobase + RBCR0;
	val	= RBCR0_INIT_VAL;		//0
	out_8(port,val);

	// RBCR1
	port	= pnet_dev->iobase + RBCR1;
	val	= RBCR1_INIT_VAL;		//0
	out_8(port,val);

	// RCR
	port	= pnet_dev->iobase + RCR;
	val	= RCR_INIT_VAL;			//0xE0
	out_8(port,val);

	// TCR
	port	= pnet_dev->iobase + TCR;
	val	= TCR_INIT_VAL;			//0xE2
	out_8(port,val);

	// TPSR
	port	= pnet_dev->iobase + TPSR;
	val	= TPSR_INIT_VAL;		//0x40
	out_8(port,val);

	// PSTART
	port	= pnet_dev->iobase + PSTART;
	val	= PSTART_INIT_VAL;		//0x4c
	out_8(port,val);

	// BNRY
	port	= pnet_dev->iobase + BNRY;
	val	= BNRY_INIT_VAL;		//0x4c
	out_8(port,val);

	// PSTOP
	port	= pnet_dev->iobase + PSTOP;
	val	= PSTOP_INIT_VAL;		//0x80
	out_8(port,val);

	// ISR
	port	= pnet_dev->iobase + ISR;
	val	= ISR_INIT_VAL;			//clear all the interrupt flag(0xFF)
	out_8(port,val);

	// IMR
	port	= pnet_dev->iobase + IMR;
	val	= IMR_INIT_VAL;			//disable all the interrupt(0x0)
	out_8(port,val);

	// DCR
	port	= pnet_dev->iobase + DCR;
	val	= DCR_INIT_VAL;			//0xC8
	out_8(port,val);

	//-------------------------------------------------------------------------
	// (PAGE1) regs setting
	//-------------------------------------------------------------------------
	set_page(PAGE1_N);
	
	// CURR
	port	= pnet_dev->iobase + CURR;
	val	= CURR_INIT_VAL;		//0x4d
	out_8(port,val);

	// setting mac(PAR0 ~ PAR5)
	port	= pnet_dev->iobase + PAR0;
	val	= PAR0_INIT_VAL;		//0xa0
	out_8(port,val);

	port	= pnet_dev->iobase + PAR1;
	val	= PAR1_INIT_VAL;		//0xb1
	out_8(port,val);

	port	= pnet_dev->iobase + PAR2;
	val	= PAR2_INIT_VAL;		//0xc2
	out_8(port,val);

	port	= pnet_dev->iobase + PAR3;
	val	= PAR3_INIT_VAL;		//0xd3
	out_8(port,val);

	port	= pnet_dev->iobase + PAR4;
	val	= PAR4_INIT_VAL;		//0xe4
	out_8(port,val);

	port	= pnet_dev->iobase + PAR5;
	val	= PAR5_INIT_VAL;		//0xf5
	out_8(port,val);

	// setting (MAR0 ~ MAR7)
	port	= pnet_dev->iobase + MAR0;
	val	= MAR0_INIT_VAL;		//0x0
	out_8(port,val);

	port	= pnet_dev->iobase + MAR1;
	val	= MAR1_INIT_VAL;		//0x41
	out_8(port,val);

	port	= pnet_dev->iobase + MAR2;
	val	= MAR2_INIT_VAL;		//0x0
	out_8(port,val);

	port	= pnet_dev->iobase + MAR3;
	val	= MAR3_INIT_VAL;		//0x80
	out_8(port,val);

	port	= pnet_dev->iobase + MAR4;
	val	= MAR4_INIT_VAL;		//0x0
	out_8(port,val);

	port	= pnet_dev->iobase + MAR5;
	val	= MAR5_INIT_VAL;		//0x0
	out_8(port,val);

	port	= pnet_dev->iobase + MAR6;
	val	= MAR6_INIT_VAL;		//0x0
	out_8(port,val);

	port	= pnet_dev->iobase + MAR7;
	val	= MAR7_INIT_VAL;		//0x0
	out_8(port,val);

	//-------------------------------------------------------------------------
	// switch to work mode
	//-------------------------------------------------------------------------
	set_page(PAGE0_N);

	// RCR
	port	= pnet_dev->iobase + RCR;
	val	= RCR_INIT_NORMAL_VAL;		//0xCC
	out_8(port,val);

	// TCR
	port	= pnet_dev->iobase + TCR;
	val	= TCR_INIT_NORMAL_VAL;		//0xE0
	out_8(port,val);

	//start working
	//set_page(PAGE0_Y);
	port	= pnet_dev->iobase + CR;
	val	= 0x22;				//
	out_8(port,val);

	// ISR
	port	= pnet_dev->iobase + ISR;
	val	= ISR_INIT_VAL;			//clear all the interrupt flag(0xFF)
	out_8(port,val);
}

//===========================================================================
//  rtl8029_send_pkg()
//
// --------------------------------
// package format:
// --------------------------------
//  dest mac : 6 bytes
//  src  mac : 6 bytes
//  type/len : 2 bytes
//  data     : <= 1500 bytes
// --------------------------------
//
//===========================================================================
PUBLIC	void 	rtl8029_send_pkg(struct s_package * pkg)
{
	u16 port;
	u8  val;
	s32 i,j;

	//-------------------------------------------------------------------------
	// start command
	//-------------------------------------------------------------------------
	set_page(PAGE0_Y);

	//-------------------------------------------------------------------------
	// setting remote DMA
	//-------------------------------------------------------------------------
	// address : RSAR0, RSAR1(total is 0x4000)
	port	= pnet_dev->iobase + RSAR0;
	val	= 0x00;				//0x00
	out_8(port,val);

	port	= pnet_dev->iobase + RSAR1;
	val	= 0x40;				//0x40
	out_8(port,val);

	// data : RBCR0, RBCR1
	port	= pnet_dev->iobase + RBCR0;
	val	= pkg->length & 0xFF;			
	out_8(port,val);

	port	= pnet_dev->iobase + RBCR1;
	val	= (pkg->length) >> 8;		
	out_8(port,val);

	//-------------------------------------------------------------------------
	// clear sending flag
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// start write RAM
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + CR;
	val	= CR_REMOTE_WRTE | CR_START_CMD; //0x12
	out_8(port,val);

	//-------------------------------------------------------------------------
	// copy package to RAM
	//-------------------------------------------------------------------------
	for(i=0; i<pkg->length; i++)
	{
		port	= pnet_dev->iobase + RDMAPORT;
		val	= pkg->buffer[i];		
		out_8(port,val);
	}

	for(i=0; i<6; i++)
	{
		port	= pnet_dev->iobase + CR;
		for(j=0; j<1000; j++)
		{
			val	= in_8(port);		
			if((val & CR_TXP) == 0)
			{
				break;
			}
		}

		port	= pnet_dev->iobase + TSR;
		val	= in_8(port);
		if((val & 0x1) != 0)
		{
			break;
		}

		//-------------------------------------------------------------------------
		// start sending package
		//-------------------------------------------------------------------------
	}
	
	//-------------------------------------------------------------------------
	// setting local DMA
	//-------------------------------------------------------------------------
	// address:  TPSR
	port	= pnet_dev->iobase + TPSR;
	val	= TPSR_INIT_VAL;		//0x40
	out_8(port,val);

	// data: TBCR0, TBCR1
	port	= pnet_dev->iobase + TBCR0;
	val	= pkg->length & 0xFF;			
	out_8(port,val);

	port	= pnet_dev->iobase + TBCR1;
	val	= (pkg->length) >> 8;			
	out_8(port,val);

	//-------------------------------------------------------------------------
	// start sending package
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + CR;
	val	= 0x3E ;			//
	out_8(port,val);

	//-------------------------------------------------------------------------
	// clear RAM finish flag
	//-------------------------------------------------------------------------
}




//===========================================================================
//  rtl8029_receive_pkg()
//
// --------------------------------
// package format(different from send package:
// --------------------------------
//  status   : 1 bytes
//  nextpage : 1 bytes
//  frame len: 2 bytes
//  dest mac : 6 bytes
//  src  mac : 6 bytes
//  type/len : 2 bytes
//  data     : <= 1500 bytes
// --------------------------------
//===========================================================================
PUBLIC	s8 	rtl8029_receive_pkg(struct s_package * pkg)
{
	u16 port, i;
	u8  val, curr, bnry;
	u8  buf_8029_header[4];
	
	//-------------------------------------------------------------------------
	// clear receiving interrupt
	//-------------------------------------------------------------------------

	// switch to page0	
	set_page(PAGE0_Y);

	port	= pnet_dev->iobase + ISR;
	val	= ISR_INIT_VAL;			//0xFF
	out_8(port,val);

	//-------------------------------------------------------------------------
	// read BNRY
	//-------------------------------------------------------------------------

	// switch to page0	
	set_page(PAGE0_Y);

	// address:  BNRY
	port	= pnet_dev->iobase + BNRY;
	bnry	= in_8(port);

	//-------------------------------------------------------------------------
	// read CURR
	//-------------------------------------------------------------------------
	
	// switch to page1	
	set_page(PAGE1_Y);

	// address:  CURR
	port	= pnet_dev->iobase + CURR;
	curr	= in_8(port);

	// switch to page0	
	set_page(PAGE0_Y);

	if(curr == 0)
	{
		return FALSE;
	}

	bnry++;	
	
	if(bnry > 0x7F)
	{
		bnry = BNRY_INIT_VAL;
	}

	if(bnry == curr)
	{
		return FALSE;
	}

	//-------------------------------------------------------------------------
	// read 4 bytes of package through remote DMA
	//-------------------------------------------------------------------------

	// switch to page0	
	set_page(PAGE0_Y);

	// data : RBCR0, RBCR1
	port	= pnet_dev->iobase + RBCR0;
	val	= 4;			
	out_8(port,val);

	port	= pnet_dev->iobase + RBCR1;
	val	= 0;		
	out_8(port,val);

	// address : RSAR0, RSAR1
	port	= pnet_dev->iobase + RSAR0;
	val	= 0;			
	out_8(port,val);

	port	= pnet_dev->iobase + RSAR1;
	val	= bnry;		
	out_8(port,val);

	//-------------------------------------------------------------------------
	// start read RAM
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + CR;
	val	= CR_REMOTE_READ | CR_START_CMD; //0xA
	out_8(port,val);

	//-------------------------------------------------------------------------
	// read data(4 bytes) from RAM into buffer
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + RDMAPORT;
	for(i=0; i<4; i++)
	{		
		buf_8029_header[i]	= in_8(port);
	}

	//-------------------------------------------------------------------------
	// setting
	//-------------------------------------------------------------------------
	u8 status	= buf_8029_header[0];
	u8 nextpage	= buf_8029_header[1];
	u8 len_h	= buf_8029_header[3];
	u8 len_l	= buf_8029_header[2];

	u16 len		= ((len_h << 8) | len_l) - 4;	// remove 4 bytes of CRC
	
	//-------------------------------------------------------------------------
	// Error package processing
	//-------------------------------------------------------------------------
	if(((status & 0x1) == 0) || (nextpage > 0x7F) || (nextpage < BNRY_INIT_VAL) || (len_h > 0x6))
	{
		set_page(PAGE1_Y);

		// get  CURR
		port	= pnet_dev->iobase + CURR;
		curr	= in_8(port);

		set_page(PAGE0_Y);
		
		bnry 	= curr - 1;

		if(bnry < BNRY_INIT_VAL)
		{
			bnry = 0x7F;
		}

		port	= pnet_dev->iobase + BNRY;
		val	= bnry;			
		out_8(port,val);

		return FALSE;
	}

	//-------------------------------------------------------------------------
	// Good package processing
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	// read package through remote DMA
	//-------------------------------------------------------------------------

	// data : RBCR0, RBCR1
	port	= pnet_dev->iobase + RBCR0;
	val	= len & 0xFF;			// the last 4 bytes is CRC, which is no use		
	out_8(port,val);

	port	= pnet_dev->iobase + RBCR1;
	val	= len >> 8;		
	out_8(port,val);

	// address : RSAR0, RSAR1
	port	= pnet_dev->iobase + RSAR0;
	val	= 4;			
	out_8(port,val);

	port	= pnet_dev->iobase + RSAR1;
	val	= bnry;		
	out_8(port,val);

	//-------------------------------------------------------------------------
	// start read RAM
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + CR;
	val	= CR_REMOTE_READ | CR_START_CMD; //0xA
	out_8(port,val);

	//-------------------------------------------------------------------------
	// read data from RAM into buffer
	//-------------------------------------------------------------------------
	port	= pnet_dev->iobase + RDMAPORT;
	for(i=0; i<(len+4); i++)
	{		
		pkg->buffer[i]	= in_8(port);
	}	

	bnry = nextpage - 1;

	if(bnry < BNRY_INIT_VAL)
	{
		bnry = 0x7F;
	}

	port	= pnet_dev->iobase + BNRY;
	val	= bnry;			
	out_8(port,val);

	port	= pnet_dev->iobase + ISR;
	val	= ISR_INIT_VAL;			//0xFF
	out_8(port,val);

	return TRUE;
}
