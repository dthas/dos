//===========================================================================
// frame.c
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
#include	"prototype.h"

//===========================================================================
// build_frame
//===========================================================================
PUBLIC	void 	build_frame(struct s_package *pkg, u16 type, u16 op, struct hwaddr dst_mac, struct iaddr dst_ip, struct hwaddr src_mac, struct iaddr src_ip)
{
	struct frame8023_header *fh 	= (struct frame8023_header *)(pkg->buffer);
	struct frame_arp_req *fc 	= (struct frame_arp_req *)(&(pkg->buffer[FRAME_HEADER_LENGTH]));
	
	//-------------------------------------------------------------------------
	// frame header
	//-------------------------------------------------------------------------
	fh->dst_mac.addr1	= dst_mac.addr1;
	fh->dst_mac.addr2	= dst_mac.addr2;
	fh->dst_mac.addr3	= dst_mac.addr3;
	fh->dst_mac.addr4	= dst_mac.addr4;
	fh->dst_mac.addr5	= dst_mac.addr5;
	fh->dst_mac.addr6	= dst_mac.addr6;

	fh->src_mac.addr1	= src_mac.addr1;
	fh->src_mac.addr2	= src_mac.addr2;
	fh->src_mac.addr3	= src_mac.addr3;
	fh->src_mac.addr4	= src_mac.addr4;
	fh->src_mac.addr5	= src_mac.addr5;
	fh->src_mac.addr6	= src_mac.addr6;

	fh->type		= big_little_16(type);

	//-------------------------------------------------------------------------
	// frame content
	//-------------------------------------------------------------------------
	fc->hw_type		= big_little_16(F_HW_TYPE);
	fc->protocol_type	= big_little_16(FRAME_IP_TYPE);
	fc->hwaddrlength	= 6;
	fc->protocoladdrlength	= 4;
	fc->op			= big_little_16(op);

	fc->src_mac.addr1	= src_mac.addr1;
	fc->src_mac.addr2	= src_mac.addr2;
	fc->src_mac.addr3	= src_mac.addr3;
	fc->src_mac.addr4	= src_mac.addr4;
	fc->src_mac.addr5	= src_mac.addr5;
	fc->src_mac.addr6	= src_mac.addr6;

	fc->src_ip.addr1	= src_ip.addr1;
	fc->src_ip.addr2	= src_ip.addr2;
	fc->src_ip.addr3	= src_ip.addr3;
	fc->src_ip.addr4	= src_ip.addr4;

	fc->dst_mac.addr1	= dst_mac.addr1;
	fc->dst_mac.addr2	= dst_mac.addr2;
	fc->dst_mac.addr3	= dst_mac.addr3;
	fc->dst_mac.addr4	= dst_mac.addr4;
	fc->dst_mac.addr5	= dst_mac.addr5;
	fc->dst_mac.addr6	= dst_mac.addr6;

	fc->dst_ip.addr1	= dst_ip.addr1;
	fc->dst_ip.addr2	= dst_ip.addr2;
	fc->dst_ip.addr3	= dst_ip.addr3;
	fc->dst_ip.addr4	= dst_ip.addr4;

	//-------------------------------------------------------------------------
	// add PAD while frame length is less than 60 bytes
	//-------------------------------------------------------------------------
	s32 i;
	for(i=42; i<60; i++)
	{
		pkg->buffer[i] = 1;
	}	

	pkg->length 	= 60;	
}

//===========================================================================
// add_frame_header
//===========================================================================
PUBLIC	void 	add_frame_header(struct s_package *pkg, u16 type, struct hwaddr dst_mac, struct hwaddr src_mac)
{
	struct frame8023_header *fh 	= (struct frame8023_header *)(pkg->buffer);
		
	//-------------------------------------------------------------------------
	// frame header
	//-------------------------------------------------------------------------
	fh->dst_mac.addr1	= dst_mac.addr1;
	fh->dst_mac.addr2	= dst_mac.addr2;
	fh->dst_mac.addr3	= dst_mac.addr3;
	fh->dst_mac.addr4	= dst_mac.addr4;
	fh->dst_mac.addr5	= dst_mac.addr5;
	fh->dst_mac.addr6	= dst_mac.addr6;

	fh->src_mac.addr1	= src_mac.addr1;
	fh->src_mac.addr2	= src_mac.addr2;
	fh->src_mac.addr3	= src_mac.addr3;
	fh->src_mac.addr4	= src_mac.addr4;
	fh->src_mac.addr5	= src_mac.addr5;
	fh->src_mac.addr6	= src_mac.addr6;

	fh->type		= big_little_16(type);	
}

//===========================================================================
// pkg_main
//===========================================================================
PUBLIC	struct s_package *	build_test_frame(struct s_package *tpkg)
{

	struct frame8023_header *f	= (struct frame8023_header *)(&(tpkg->buffer[0]));

	f->dst_mac.addr1= 0x12;
	f->dst_mac.addr2= 0x22;
	f->dst_mac.addr3= 0x32;
	f->dst_mac.addr4= 0x42;
	f->dst_mac.addr5= 0x52;
	f->dst_mac.addr6= 0x62;

	f->src_mac.addr1= 0xa1;
	f->src_mac.addr2= 0xb1;
	f->src_mac.addr3= 0xc1;
	f->src_mac.addr4= 0xd1;
	f->src_mac.addr5= 0xe1;
	f->src_mac.addr6= 0xf1;

	f->type		= FRAME_ARP_TYPE;	//0x0608

	struct frame_arp_req *arpr 	= (struct frame_arp_req *)(&(tpkg->buffer[FRAME_HEADER_LENGTH]));

	arpr->hw_type		= 0x0100;
	arpr->protocol_type	= 0x0008;
	arpr->hwaddrlength	= 6;
	arpr->protocoladdrlength= 4;
	arpr->op		= 0x0100;

	arpr->src_mac.addr1	= 0xa1;
	arpr->src_mac.addr2	= 0xb1;
	arpr->src_mac.addr3	= 0xc1;
	arpr->src_mac.addr4	= 0xd1;
	arpr->src_mac.addr5	= 0xe1;
	arpr->src_mac.addr6	= 0xf1;

	arpr->src_ip.addr1	= 101;
	arpr->src_ip.addr2	= 102;
	arpr->src_ip.addr3	= 103;
	arpr->src_ip.addr4	= 104;

	arpr->dst_mac.addr1	= 0x12;
	arpr->dst_mac.addr2	= 0x22;
	arpr->dst_mac.addr3	= 0x32;
	arpr->dst_mac.addr4	= 0x42;
	arpr->dst_mac.addr5	= 0x52;
	arpr->dst_mac.addr6	= 0x62;

	arpr->dst_ip.addr1	= 192;
	arpr->dst_ip.addr2	= 168;
	arpr->dst_ip.addr3	= 131;
	arpr->dst_ip.addr4	= 22;

	s32 i;
	for(i=42; i<60; i++)
	{
		tpkg->buffer[i] = 1;
	}	

	tpkg->length 	= 60;

	return tpkg;
}
