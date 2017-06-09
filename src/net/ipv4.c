//===========================================================================
// ipv4.c
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
#include	"udp.h"
#include	"ipv4.h"
#include	"prototype.h"

//===========================================================================
// add_ipv4_header
//===========================================================================
PUBLIC	void 	add_ipv4_header(struct s_package *pkg, struct iaddr src_ip, u8 ttl, struct iaddr dst_ip, u8 protocol, u8 tos, u16 total_len, u16 offset, u8 flag)
{
	struct s_ipv4_header * iph = (struct s_ipv4_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH]));

	iph->version_len	= (IP_VERSION_4 << 4) | IP_HEADER_LEN_IN_BYTE;
	iph->tos		= tos;
	iph->length		= total_len;
	iph->iden		= 0;
	
	iph->flag_offset	= big_little_16((flag << 13) | offset) ;

	iph->ttl		= ttl;
	iph->protocol		= protocol;
	iph->checksum		= 0;

	iph->src_ip.addr1	= src_ip.addr1; 
	iph->src_ip.addr2	= src_ip.addr2; 
	iph->src_ip.addr3	= src_ip.addr3; 
	iph->src_ip.addr4	= src_ip.addr4; 

	iph->dst_ip.addr1	= dst_ip.addr1;
	iph->dst_ip.addr2	= dst_ip.addr2;
	iph->dst_ip.addr3	= dst_ip.addr3;
	iph->dst_ip.addr4	= dst_ip.addr4;

	iph->checksum		= makechksum(iph,IPV4_HEADER_LENGTH);
}

//===========================================================================
// ip_forward_v4
//===========================================================================
PUBLIC	void 	ip_forward_v4(struct s_package *pkg)
{
	struct s_ipv4_header * iph = (struct s_ipv4_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH]));

	if((iph->ttl == 0) || (iph->ttl == 1))
	{
		return ;
	}

	u8 ttl			= iph->ttl - 1;
	struct iaddr src_ip	= iph->src_ip;
	struct iaddr dst_ip	= iph->dst_ip;
	u8 protocol		= iph->protocol;
	u8 tos			= iph->tos;
	u16 total_len		= iph->length;
	u16 offset		= (iph->flag_offset)&0x1FFF;
	u8 flag			= (iph->flag_offset>>13)&0x7;

	add_ipv4_header(pkg, src_ip, ttl, dst_ip, protocol, tos, total_len, offset, flag);

	rtl8029_send_pkg(pkg);
}

//===========================================================================
// ip_process
//===========================================================================
PUBLIC	void 	ip_process(struct s_package *pkg, struct iaddr src_ip, struct iaddr dst_ip, u8 protocol, u16 data_length)
{
	ipv4_process(pkg, src_ip, dst_ip, protocol, data_length);
}

//===========================================================================
// ip_process
//===========================================================================
PUBLIC	void 	ipv4_process(struct s_package *src_pkg, struct iaddr src_ip, struct iaddr dst_ip, u8 protocol, u16 total_data_len)
{
	s16	t_offset, t_ip_len, t_data_len;
	s32	data_len	= total_data_len;	// max(data_len) is 64k
	u8	flag;

	//-------------------------------------------------------------------------
	// no fragment
	//-------------------------------------------------------------------------
	if(data_len <= IP_DATA_MAX)
	{
		t_offset	= 0;
		t_data_len	= data_len;
		flag		= IP_FLAG_NO_FRAGMENT;
		t_ip_len	= IPV4_HEADER_LENGTH + t_data_len;

		ipv4_send(src_pkg, src_ip, IP_TTL, dst_ip, protocol, 0, t_ip_len, t_offset,  flag);

		return;
	}

	//-------------------------------------------------------------------------
	// fragments
	//-------------------------------------------------------------------------
	u16	total_header_len;
	s8	*dst, *src;
	u32	len;
	u16 	src_port, dst_port, udp_len;
	
		
	if(protocol == PROTOCOL_UDP)
	{
		total_header_len= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH;
		data_len	= data_len - UDP_HEADER_LENGTH;	//exclude the udp_h		

		struct s_udp_header *udph 	= (struct s_udp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

		src_port	= big_little_16(udph->src_port);
		dst_port	= big_little_16(udph->dst_port);		
		udp_len		= big_little_16(udph->udp_len);				
	}

		
	for(t_offset=0, t_ip_len=0, t_data_len=0, flag=0; data_len >0 ; data_len -= IP_DATA_MAX, t_offset += t_data_len)
	{
		struct s_package d_pkg, *dst_pkg;
		dst_pkg	= &d_pkg;

		if(data_len > IP_DATA_MAX)
		{
			t_data_len	= IP_DATA_MAX;
			flag		= IP_FLAG_MAY_FRAGMENT;
		}
		else
		{
			t_data_len	= data_len;
			flag		= IP_FLAG_LAST_FRAGMENT;
		}

		//-------------------------------------------------------------------------
		// //copy the data
		//-------------------------------------------------------------------------
		dst		= (s8*)(&(dst_pkg->buffer[total_header_len]));
		src		= (s8*)(&(src_pkg->buffer[total_header_len + t_offset]));
		len		= t_data_len;
		strcpy(dst, src, len);		//copy the data

		if(protocol == PROTOCOL_UDP)
		{
			udp_len	= UDP_HEADER_LENGTH + t_data_len;
		
			add_udp_header(dst_pkg, src_ip, udp_len, dst_ip, protocol, src_port, dst_port);
			
			t_ip_len	= IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + t_data_len;
		}
		else
		{
			t_ip_len	= IPV4_HEADER_LENGTH + t_data_len;
		}

		//-------------------------------------------------------------------------
		// send the pkg
		//-------------------------------------------------------------------------
		ipv4_send(dst_pkg, src_ip, IP_TTL, dst_ip, protocol, 0, t_ip_len, (t_offset/8),  flag);		
	}	
}

//===========================================================================
// ipv4_send
//===========================================================================
PUBLIC	void 	ipv4_send(struct s_package *pkg, struct iaddr src_ip, u8 ttl, struct iaddr dst_ip, u8 protocol, u8 tos, u16 ip_len, u16 offset, u8 flag)
{
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	struct route_table *rtl = search_route(dst_ip);

	if(!rtl)
	{
		printk("ip: route check fail");
		printk("\n");

		return;
	}
	
	add_ipv4_header(pkg, src_ip, IP_TTL, dst_ip, protocol, 0, big_little_16(ip_len), offset,  flag);

	//-------------------------------------------------------------------------
	// add frame header
	//-------------------------------------------------------------------------
	add_frame_header(pkg, FRAME_IP_TYPE, rtl->gw_mac, pnet_dev->mac);

	//-------------------------------------------------------------------------
	// setting the whole package length
	//-------------------------------------------------------------------------
	pkg->length 	= FRAME_HEADER_LENGTH + ip_len ; //	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);	
}
