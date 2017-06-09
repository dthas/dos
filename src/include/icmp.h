//===========================================================================
// icmp.h
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
#ifndef	_X_ICMP_H_
#define	_X_ICMP_H_

#define ICMP_HEADER_LENGTH_0_8		8

struct s_icmp_header
{
	u8 type;
	u8 code;
	u16 checksum;
	u16 iden;
	u16 seq;
};

struct s_icmp_timestamp
{
	u32 send_timestamp;
	u32 recv_timestamp;
	u32 tran_timestamp;
};

struct s_icmp_netmask
{
	u32 netmask;
};

#endif
