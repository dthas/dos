//===========================================================================
// igmp.h
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
#ifndef	_X_IGMP_H_
#define	_X_IGMP_H_

#define IGMP_H_LEN		12

#define IGMP_TYPE_QUERY		0x11
#define IGMP_TYPE_REPORT	0x22

#define IGMP_QUERY_RESV		0x0
#define IGMP_QUERY_S		0x0
#define IGMP_QUERY_QRV		0x0

struct s_igmp_query_h
{
	u8 type;
	u8 res_code;
	u16 chksum;
	struct iaddr group_addr;
	u8 resv_s_qrv;
	u8 qqic;
	u16 nr_src;	
};

#endif
