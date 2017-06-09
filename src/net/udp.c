//===========================================================================
// udp.c
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
#include	"ipv4.h"
#include	"udp.h"
#include	"prototype.h"

PRIVATE	s8 	*tmp_udp_data = NET_TEMP_BUF;
//===========================================================================
// add_udp_header
//===========================================================================
PUBLIC	void 	add_udp_header(struct s_package *pkg, struct iaddr src_ip, u16 udp_len, struct iaddr dst_ip, u8 protocol,u16 src_port, u16 dst_port)
{
	
	//-------------------------------------------------------------------------
	// make checksum
	//-------------------------------------------------------------------------

	//1)build test space
	s8 *dst_addr	= &tmp_udp_data[UDP_HEADER_LENGTH + PUDP_HEADER_LENGTH];
	s8 *src_addr	= &(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s32 len		= udp_len - UDP_HEADER_LENGTH;

	strcpy(dst_addr, src_addr, len);

	struct s_g_udp_header *g = (struct s_g_udp_header *)tmp_udp_data;

	//2)create udp psedu-header
	struct s_pudp_header *p = &(g->p);
	p->src_ip	= src_ip;
	p->dst_ip	= dst_ip;
	p->zero		= 0;
	p->protocol	= protocol;
	p->udp_len	= big_little_16(udp_len);

	//3) create udp header
	struct s_udp_header *u = &(g->u);
	u->src_port	= big_little_16(src_port);
	u->dst_port	= big_little_16(dst_port);
	u->udp_len	= big_little_16(udp_len);
	u->checksum	= 0;
	
	u->checksum	= makechksum(tmp_udp_data,(udp_len+PUDP_HEADER_LENGTH));	

	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------

	struct s_udp_header * udp = (struct s_udp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	udp->src_port	= big_little_16(src_port);
	udp->dst_port	= big_little_16(dst_port);
	udp->udp_len	= big_little_16(udp_len);
	udp->checksum	= u->checksum;	
}
