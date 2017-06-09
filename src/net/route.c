//===========================================================================
// route.c
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
#include	"rtl8029.h"
#include	"frame.h"
#include	"net.h"
#include	"route.h"
#include	"prototype.h"

PRIVATE int find_slot();
PRIVATE struct route_table *search_gateway(struct iaddr src_ip);

PRIVATE	struct route_table g_routetbl[ROUTES_IN_TABLE_MAX];
PRIVATE u8 route_count;

//===========================================================================
// PUBLIC	void	init_routetbl()
//===========================================================================
PUBLIC	void	init_routetbl()
{
	sys_routetbl 	= g_routetbl;
	route_count	= 0;		//how many routes are in g_routetbl(the max is 100)

	struct route_table tmp_rtbl;

	//-------------------------------------------------------------------------
	// add a route
	//-------------------------------------------------------------------------
	tmp_rtbl.dst_ip.addr1		= 192;
	tmp_rtbl.dst_ip.addr2		= 168;
	tmp_rtbl.dst_ip.addr3		= 1;
	tmp_rtbl.dst_ip.addr4		= 3;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 140;

	tmp_rtbl.gw_mac.addr1		= 0x00;
	tmp_rtbl.gw_mac.addr2		= 0x1a;
	tmp_rtbl.gw_mac.addr3		= 0x4d;
	tmp_rtbl.gw_mac.addr4		= 0x66;
	tmp_rtbl.gw_mac.addr5		= 0xd4;
	tmp_rtbl.gw_mac.addr6		= 0xF9;

	tmp_rtbl.metric			= 1;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 0;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// add a route
	//-------------------------------------------------------------------------
	tmp_rtbl.dst_ip.addr1		= pnet_dev->ip.addr1;
	tmp_rtbl.dst_ip.addr2		= pnet_dev->ip.addr2;
	tmp_rtbl.dst_ip.addr3		= pnet_dev->ip.addr3;
	tmp_rtbl.dst_ip.addr4		= pnet_dev->ip.addr4;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 1;

	tmp_rtbl.gw_mac.addr1		= 0xFF;
	tmp_rtbl.gw_mac.addr2		= 0xFF;
	tmp_rtbl.gw_mac.addr3		= 0xFF;
	tmp_rtbl.gw_mac.addr4		= 0xFF;
	tmp_rtbl.gw_mac.addr5		= 0xFF;
	tmp_rtbl.gw_mac.addr6		= 0xFF;

	tmp_rtbl.metric			= 1;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 1;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// add a route(smtp gmail)
	//-------------------------------------------------------------------------
	tmp_rtbl.dst_ip.addr1		= 173;
	tmp_rtbl.dst_ip.addr2		= 194;
	tmp_rtbl.dst_ip.addr3		= 79;
	tmp_rtbl.dst_ip.addr4		= 26;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 1;

	tmp_rtbl.gw_mac.addr1		= 0xEC;
	tmp_rtbl.gw_mac.addr2		= 0x88;
	tmp_rtbl.gw_mac.addr3		= 0x8F;
	tmp_rtbl.gw_mac.addr4		= 0xF5;
	tmp_rtbl.gw_mac.addr5		= 0xBD;
	tmp_rtbl.gw_mac.addr6		= 0xFE;	

	tmp_rtbl.metric			= 64;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 0;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// add a route(pop3 gmail)
	//-------------------------------------------------------------------------
	tmp_rtbl.dst_ip.addr1		= 74;
	tmp_rtbl.dst_ip.addr2		= 125;
	tmp_rtbl.dst_ip.addr3		= 141;
	tmp_rtbl.dst_ip.addr4		= 108;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 1;

	tmp_rtbl.gw_mac.addr1		= 0xEC;
	tmp_rtbl.gw_mac.addr2		= 0x88;
	tmp_rtbl.gw_mac.addr3		= 0x8F;
	tmp_rtbl.gw_mac.addr4		= 0xF5;
	tmp_rtbl.gw_mac.addr5		= 0xBD;
	tmp_rtbl.gw_mac.addr6		= 0xFE;	

	tmp_rtbl.metric			= 64;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 0;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// add a route
	//-------------------------------------------------------------------------
	tmp_rtbl.dst_ip.addr1		= 255;
	tmp_rtbl.dst_ip.addr2		= 255;
	tmp_rtbl.dst_ip.addr3		= 255;
	tmp_rtbl.dst_ip.addr4		= 255;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 1;

	tmp_rtbl.gw_mac.addr1		= 0xFF;
	tmp_rtbl.gw_mac.addr2		= 0xFF;
	tmp_rtbl.gw_mac.addr3		= 0xFF;
	tmp_rtbl.gw_mac.addr4		= 0xFF;
	tmp_rtbl.gw_mac.addr5		= 0xFF;
	tmp_rtbl.gw_mac.addr6		= 0xFF;

	tmp_rtbl.metric			= 1;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 0;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// add a route
	//-------------------------------------------------------------------------	
	tmp_rtbl.dst_ip.addr1		= 202;
	tmp_rtbl.dst_ip.addr2		= 96;
	tmp_rtbl.dst_ip.addr3		= 128;
	tmp_rtbl.dst_ip.addr4		= 166;

	tmp_rtbl.dst_netmask.addr1	= 255;
	tmp_rtbl.dst_netmask.addr2	= 255;
	tmp_rtbl.dst_netmask.addr3	= 255;
	tmp_rtbl.dst_netmask.addr4	= 0;

	tmp_rtbl.gw_ip.addr1		= 192;
	tmp_rtbl.gw_ip.addr2		= 168;
	tmp_rtbl.gw_ip.addr3		= 1;
	tmp_rtbl.gw_ip.addr4		= 1;

	tmp_rtbl.gw_mac.addr1		= 0xEC;
	tmp_rtbl.gw_mac.addr2		= 0x88;
	tmp_rtbl.gw_mac.addr3		= 0x8F;
	tmp_rtbl.gw_mac.addr4		= 0xF5;
	tmp_rtbl.gw_mac.addr5		= 0xBD;
	tmp_rtbl.gw_mac.addr6		= 0xFE;	

	tmp_rtbl.metric			= 64;
	tmp_rtbl.fl_used		= 1;

	tmp_rtbl.fl_default		= 0;

	add_route(&tmp_rtbl);

	//-------------------------------------------------------------------------
	// update gateway mac on routetable
	//-------------------------------------------------------------------------
	get_routemac(sys_routetbl);
	
}

//===========================================================================
// PUBLIC	void	add_route()
//===========================================================================
PUBLIC	void	add_route(struct route_table *rtbl)
{
	s32 freeslot = find_slot();

	if(!freeslot)
	{
		printk("route table is full, operation fail");
		printk("\n");
	}
	else
	{
		struct route_table *sys_rtbl = &g_routetbl[freeslot-1];

		sys_rtbl->dst_ip.addr1 		= rtbl->dst_ip.addr1;
		sys_rtbl->dst_ip.addr2 		= rtbl->dst_ip.addr2;
		sys_rtbl->dst_ip.addr3		= rtbl->dst_ip.addr3;
		sys_rtbl->dst_ip.addr4		= rtbl->dst_ip.addr4;

		sys_rtbl->gw_ip.addr1		= rtbl->gw_ip.addr1;
		sys_rtbl->gw_ip.addr2		= rtbl->gw_ip.addr2;
		sys_rtbl->gw_ip.addr3		= rtbl->gw_ip.addr3;
		sys_rtbl->gw_ip.addr4		= rtbl->gw_ip.addr4;

		sys_rtbl->dst_netmask.addr1	= rtbl->dst_netmask.addr1;
		sys_rtbl->dst_netmask.addr2	= rtbl->dst_netmask.addr2;
		sys_rtbl->dst_netmask.addr3	= rtbl->dst_netmask.addr3;
		sys_rtbl->dst_netmask.addr4	= rtbl->dst_netmask.addr4;

		sys_rtbl->gw_mac.addr1  	= rtbl->gw_mac.addr1;
		sys_rtbl->gw_mac.addr2  	= rtbl->gw_mac.addr2;
		sys_rtbl->gw_mac.addr3  	= rtbl->gw_mac.addr3;
		sys_rtbl->gw_mac.addr4  	= rtbl->gw_mac.addr4;
		sys_rtbl->gw_mac.addr5  	= rtbl->gw_mac.addr5;
		sys_rtbl->gw_mac.addr6  	= rtbl->gw_mac.addr6;

		sys_rtbl->metric	 	= rtbl->metric;
		sys_rtbl->fl_used	 	= rtbl->fl_used;
		
		sys_rtbl->fl_default		= rtbl->fl_default;

		route_count++;
	}
}

//===========================================================================
// PUBLIC	void	del_route()
//===========================================================================
PUBLIC	void	del_route(struct route_table *rtbl)
{
	if(route_count == 0)
	{
		printk("route table is empty, openation fail\n");
	}
	else
	{
		u8 i;
		struct route_table *sys_rtbl = g_routetbl;

		for(i=route_count; (i>=0) && (sys_rtbl->fl_used == 1); i--, sys_rtbl++)
		{
			if((rtbl->dst_ip.addr1 == sys_rtbl->dst_ip.addr1) &&
			   (rtbl->dst_ip.addr2 == sys_rtbl->dst_ip.addr2) &&	
			   (rtbl->dst_ip.addr3 == sys_rtbl->dst_ip.addr3) &&	
			   (rtbl->dst_ip.addr4 == sys_rtbl->dst_ip.addr4) &&
			   (rtbl->dst_netmask.addr1  == sys_rtbl->dst_netmask.addr1)  &&
			   (rtbl->dst_netmask.addr2  == sys_rtbl->dst_netmask.addr2)  &&
			   (rtbl->dst_netmask.addr3  == sys_rtbl->dst_netmask.addr3)  &&
			   (rtbl->dst_netmask.addr4  == sys_rtbl->dst_netmask.addr4)  &&
			   (rtbl->gw_ip.addr1  == sys_rtbl->gw_ip.addr1)  &&
			   (rtbl->gw_ip.addr2  == sys_rtbl->gw_ip.addr2)  &&
			   (rtbl->gw_ip.addr3  == sys_rtbl->gw_ip.addr3)  &&
			   (rtbl->gw_ip.addr4  == sys_rtbl->gw_ip.addr4) 
			  )
			{
				sys_rtbl->fl_used = 0;

				route_count--;

				return;
			} 	
		}

		printk("route doesn't exist, no routes will be delete");
		printk("\n");
	}
}

//===========================================================================
// PUBLIC struct route_table *search_route(struct iaddr *src_ip)
//===========================================================================
PUBLIC struct route_table *search_route(struct iaddr src_ip)
{
	if(route_count == 0)
	{
		printk("route table is empty, openation fail");
		printk("\n");
		return 0;
	}
	else
	{
		u8 i;
		struct route_table *sys_rtbl = g_routetbl;
		u32 srcip, dstip, netmask; 
				
		for(i=0; (i < route_count) && (sys_rtbl->fl_used == 1); i++,sys_rtbl++)
		{			
			if(is_same_net(src_ip, sys_rtbl->dst_netmask, sys_rtbl->dst_ip, sys_rtbl->dst_netmask))
			{
				return sys_rtbl;
			}
		}
	}

	return 0;
}


//===========================================================================
// PUBLIC	void	get_routemac
//===========================================================================
PUBLIC	void	get_routemac(struct route_table *rtbl)
{
	struct hwaddr dst_mac; 
	struct hwaddr src_mac; 
	struct iaddr  src_ip;
	struct iaddr  *dst_ip;
	struct s_package pkg;

	dst_mac.addr1	= 0xFF;
	dst_mac.addr2	= 0xFF;
	dst_mac.addr3	= 0xFF;
	dst_mac.addr4	= 0xFF;
	dst_mac.addr5	= 0xFF;
	dst_mac.addr6	= 0xFF;

	src_mac.addr1	= pnet_dev->mac.addr1;
	src_mac.addr2	= pnet_dev->mac.addr2;
	src_mac.addr3	= pnet_dev->mac.addr3;
	src_mac.addr4	= pnet_dev->mac.addr4;
	src_mac.addr5	= pnet_dev->mac.addr5;
	src_mac.addr6	= pnet_dev->mac.addr6;

	src_ip.addr1	= pnet_dev->ip.addr1;
	src_ip.addr2	= pnet_dev->ip.addr2;
	src_ip.addr3	= pnet_dev->ip.addr3;
	src_ip.addr4	= pnet_dev->ip.addr4;
	
	s32 i;
	struct route_table *t_rtbl = rtbl;
	for(i=0; (i<route_count) && t_rtbl ; i++, t_rtbl++)
	{
		if(!chk_src_dest_ip(&src_ip, &(t_rtbl->gw_ip)))
		{
			dst_ip	= &(t_rtbl->gw_ip);
		}
		else
		{
			dst_ip	= &(t_rtbl->dst_ip);
		}

		build_frame(&pkg, FRAME_ARP_TYPE, F_OP_ARP_REQUEST, dst_mac, *dst_ip, src_mac, src_ip);

		rtl8029_send_pkg(&pkg);		
	}
}

//===========================================================================
// find_slot()
//===========================================================================
PRIVATE int find_slot()
{
	int i;
	struct route_table *rtbl = g_routetbl;

	for(i=1; i<(ROUTES_IN_TABLE_MAX); i++, rtbl++)
	{
		if(rtbl->fl_used != 1)
		{
			return i;
		}
	}

	return 0;
}

//===========================================================================
// find_slot()
//===========================================================================
PRIVATE struct route_table *search_gateway(struct iaddr src_ip)
{
	if(route_count == 0)
	{
		printk("route table is empty, openation fail");
		printk("\n");
		return 0;
	}
	else
	{
		u8 i;
		struct route_table *sys_rtbl = g_routetbl;
		u32 srcip, dstip, netmask; 
				
		for(i=0; (i < route_count) && (sys_rtbl->fl_used == 1); i++,sys_rtbl++)
		{			
			if(is_same_net(src_ip, sys_rtbl->dst_netmask, sys_rtbl->gw_ip, sys_rtbl->dst_netmask))
			{
				if(chk_src_dest_ip(&(sys_rtbl->gw_ip), &src_ip))
				{
					return sys_rtbl;
				}
			}
		}
	}

	return 0;
}

//===========================================================================
// update_routetable(struct iaddr gateway_ip, struct hwaddr gmac)
//===========================================================================
PUBLIC 	void 	update_routetable(struct iaddr gateway_ip, struct hwaddr gmac)
{
	struct route_table *rtbl;
	struct iaddr *src_ip = &(pnet_dev->ip);

	rtbl = search_gateway(gateway_ip);
	if(rtbl)
	{
		rtbl->gw_mac.addr1	= gmac.addr1;
		rtbl->gw_mac.addr2	= gmac.addr2;
		rtbl->gw_mac.addr3	= gmac.addr3;
		rtbl->gw_mac.addr4	= gmac.addr4;
		rtbl->gw_mac.addr5	= gmac.addr5;
		rtbl->gw_mac.addr6	= gmac.addr6;		
	}
}

//===========================================================================
// PUBLIC	u8	is_same_net
//===========================================================================
PUBLIC	u8	is_same_net(struct iaddr src_ip, struct iaddr src_mask, struct iaddr dst_ip, struct iaddr dst_mask)
{
	u32 srcip, dstip, srcmask, dstmask;

	srcip = dstip = srcmask = dstmask = 0;
			
	srcip = src_ip.addr1<<24|src_ip.addr2<<16|src_ip.addr3<<8|src_ip.addr4;
	dstip = dst_ip.addr1<<24|dst_ip.addr2<<16|dst_ip.addr3<<8|dst_ip.addr4;

	srcmask = src_mask.addr1<<24 |src_mask.addr2<<16 |src_mask.addr3<<8 |src_mask.addr4;
	dstmask = dst_mask.addr1<<24 |dst_mask.addr2<<16 |dst_mask.addr3<<8 |dst_mask.addr4;
						

	dstip &= dstmask;
	srcip &= srcmask;

	if(srcip == dstip)
	{
		return 1;
	}
}



//===========================================================================
// PUBLIC struct iaddr * get_default_gw()
//===========================================================================
PUBLIC struct iaddr get_default_gw()
{
	u8 i;
	struct route_table *sys_rtbl = g_routetbl;
					
	for(i=0; (i < route_count) && (sys_rtbl->fl_used == 1); i++,sys_rtbl++)
	{
		if(sys_rtbl->fl_default == 1)
		{
			return sys_rtbl->gw_ip;
		}
	}	
}

//===========================================================================
// PUBLIC	void	pr_routetbl()
//===========================================================================
PUBLIC	void	pr_routetbl()
{
	int i;
	struct route_table *rtbl = g_routetbl;

	printk("\nRoute table :%d routes\n", route_count);

	printk("===============================================================================\n");
	printk("Destination        Gateway             Netmask        GateMac                  \n");
	printk("===============================================================================\n");

	for(i=0; (i<ROUTES_IN_TABLE_MAX) && (rtbl->fl_used == 1); i++, rtbl++)
	{	
		printk("%d.%d.%d.%d ",(rtbl->dst_ip.addr1),(rtbl->dst_ip.addr2),(rtbl->dst_ip.addr3), (rtbl->dst_ip.addr4));
		printk(" %d.%d.%d.%d ",(rtbl->gw_ip.addr1),(rtbl->gw_ip.addr2),(rtbl->gw_ip.addr3), (rtbl->gw_ip.addr4));
		printk(" %d.%d.%d.%d ",(rtbl->dst_netmask.addr1),(rtbl->dst_netmask.addr2),(rtbl->dst_netmask.addr3), (rtbl->dst_netmask.addr4));
		printk(" %x:%x:%x:%x:%x:%x\n",(rtbl->gw_mac.addr1),(rtbl->gw_mac.addr2),(rtbl->gw_mac.addr3), (rtbl->gw_mac.addr4),(rtbl->gw_mac.addr5),(rtbl->gw_mac.addr6));
	}

	printk("------------------------------------------------------------------------------\n");
}
