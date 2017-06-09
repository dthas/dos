//===========================================================================
// udp.h
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
#ifndef	_X_UDP_H_
#define	_X_UDP_H_

#define UDP_HEADER_LENGTH		8
#define PUDP_HEADER_LENGTH		20

struct s_udp_header
{
	u16 src_port;
	u16 dst_port;
	u16 udp_len;
	u16 checksum;
};

struct s_pudp_header
{
	struct iaddr src_ip;
	struct iaddr dst_ip;
	u8 zero;
	u8 protocol;
	u16 udp_len;	
};

struct s_g_udp_header
{
	struct s_pudp_header p;
	struct s_udp_header  u;
};

#endif
