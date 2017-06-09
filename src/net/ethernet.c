//===========================================================================
// ethernet.c
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
#include	"frame.h"
#include	"net.h"
#include	"rtl8029.h"
#include	"ethernet.h"
#include	"prototype.h"

PRIVATE void set_macaddr();
PRIVATE void set_ipaddr();
PRIVATE void set_netmask();

//===========================================================================
// init tty
//===========================================================================
PUBLIC	void init_ethernet()
{
	set_macaddr();

	set_ipaddr();
		
	set_netmask();
}

//===========================================================================
// init tty
//===========================================================================
PRIVATE void set_macaddr()
{
	pnet_dev->mac.addr1 	= PAR0_INIT_VAL;	
	pnet_dev->mac.addr2 	= PAR1_INIT_VAL;	
	pnet_dev->mac.addr3 	= PAR2_INIT_VAL;	
	pnet_dev->mac.addr4 	= PAR3_INIT_VAL;	
	pnet_dev->mac.addr5 	= PAR4_INIT_VAL;	
	pnet_dev->mac.addr6 	= PAR5_INIT_VAL;
}

//===========================================================================
// init tty
//===========================================================================
PRIVATE void set_ipaddr()
{
	pnet_dev->ip.addr1 	= 192;
	pnet_dev->ip.addr2 	= 168;
	pnet_dev->ip.addr3 	= 1;
	pnet_dev->ip.addr4 	= 140;
}

//===========================================================================
// init tty
//===========================================================================
PRIVATE void set_netmask()
{
	pnet_dev->netmask.addr1 	= 255;
	pnet_dev->netmask.addr2 	= 255;
	pnet_dev->netmask.addr3 	= 255;
	pnet_dev->netmask.addr4 	= 0;
}

//===========================================================================
// init tty
//===========================================================================
PUBLIC void pr_ethernet()
{
	printk("iobase=%x, irq=%d, mac=%x:%x:%x:%x:%x:%x, ip=%d:%d:%d:%d\n", pnet_dev->iobase, pnet_dev->irq, pnet_dev->mac.addr1, pnet_dev->mac.addr2, pnet_dev->mac.addr3, pnet_dev->mac.addr4 , pnet_dev->mac.addr5, pnet_dev->mac.addr6, pnet_dev->ip.addr1, pnet_dev->ip.addr2, pnet_dev->ip.addr3, pnet_dev->ip.addr4);	
}
