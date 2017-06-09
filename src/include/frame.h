//===========================================================================
// frame.h
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
#ifndef	_X_FRAME_H_
#define	_X_FRAME_H_

#define F_MAC_DEST_OFFSET		0
#define F_IP_SRC_OFFSET			6
#define F_FRAME_TYPE_OFFSET		12
#define F_HW_TYPE_OFFSET		14
#define F_PROTOCOL_TYPE_OFFSET		16
#define F_HW_ADDRLEN_OFFSET		18
#define F_PROTOCOL_ADDRLEN_OFFSET	19
#define F_OP_OFFSET			20
#define F_MAC_SRC_OFFSET		22
#define F_IP_SRC_OFFSET			28

#define F_HW_TYPE			0x0001

#define FRAME_IP_TYPE			0x0800		//0x0800
#define FRAME_ARP_TYPE			0x0806		//0x0806
#define FRAME_RARP_TYPE			0x0835		//0x0835

#define F_OP_ARP_REQUEST		1
#define F_OP_ARP_REPLY			2
#define F_OP_RARP_REQUEST		3
#define F_OP_RARP_REPLY			4

#define FRAME_HEADER_LENGTH		14

//===========================================================================
// variable
//===========================================================================
struct frame8023_header
{
	struct hwaddr dst_mac; 
	struct hwaddr src_mac; 
        u16 type;       
};

struct frame8023_8029_header
{
	u8 status; 
	u8 nextpage;         
	u8 len_l;
	u8 len_h;	      
};

struct frame_arp_req
{
	u16 hw_type;
	u16 protocol_type;
	u8  hwaddrlength;
	u8  protocoladdrlength;
	u16 op;
	struct hwaddr src_mac; 
	struct iaddr src_ip;
	struct hwaddr dst_mac; 
	struct iaddr dst_ip;	       
};

#endif
