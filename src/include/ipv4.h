//===========================================================================
// ipv4.h
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
#ifndef	_X_IPV4_H_
#define	_X_IPV4_H_

#define	IP_VERSION_4		0x4
#define	IP_HEADER_LEN_IN_BYTE	0x5	//5 * 4 = 20 BYTES
#define	IP_TTL			64

#define	IP_FLAG_LAST_FRAGMENT	0x0
#define	IP_FLAG_MAY_FRAGMENT	0x1
#define	IP_FLAG_NO_FRAGMENT	0x2


#define IPV4_HEADER_LENGTH	20
#define IPV4_DATA_MIN_LENGTH	512

struct s_ipv4_header
{
	u8 version_len;
	u8 tos;
	u16 length;
	u16 iden;
	u16 flag_offset;
	u8 ttl;
	u8 protocol;
	u16 checksum;
	struct iaddr src_ip;
	struct iaddr dst_ip;
};

#endif
