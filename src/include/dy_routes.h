//===========================================================================
// dy_routes.h
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
#ifndef	_X_DYROUTES_H_
#define	_X_DYROUTES_H_

#define AS_ID			1001

#define DYROUTES_PORT		520
#define RIPH_LEN		4

#define RIP_CMD_REQ		1
#define RIP_CMD_REPLY		2
#define RIP_CMD_POLL		5
#define RIP_CMD_POLL_ENTRY	6

#define RIP_VERSION_1		1
#define RIP_VERSION_2		2

#define OSPF_VERSION		2
#define OSPF_H_LEN		24
#define OSPF_C_LEN		20

#define OSPF_TYPE_HELLO		1
#define OSPF_TYPE_DATABASE	2
#define OSPF_TYPE_LSA		3
#define OSPF_TYPE_LSU		4
#define OSPF_TYPE_LSC		5

#define OSPF_IDENTIFY_N		0
#define OSPF_IDENTIFY_PW	1


#define BGP_H_LEN		19
#define BGP_C_KEEPALIVE_LEN	0

#define BGP_TYPE_OPEN		1
#define BGP_TYPE_UPDATE		2
#define BGP_TYPE_KEEPALIVE	3
#define BGP_TYPE_INFORM		4

struct s_rip1_h
{
	u8 cmd;
	u8 version;
	u16 zero1;	
};

struct s_rip1_c
{
	u16 addr;
	u16 zero2;
	struct iaddr ip;
	u32 zero3;
	u32 zero4;
	u32 metric;
};


struct s_rip2_h
{
	u8 cmd;
	u8 version;
	u16 routedomain;	
};

struct s_rip2_c
{
	u16 addr;
	u16 routemark;
	struct iaddr ip;
	struct iaddr netmask;
	struct iaddr next_ip;
	u32 metric;
};

struct s_ospf_h
{
	u8 version;	
	u8 type;
	u16 length;
	struct iaddr src_ip;
	u32 area_id;
	u16 chksum;
	u16 iden_type;
	u32 identify_0_3;
	u32 identify_4_7;
};

struct s_ospf_c_hello
{
	struct iaddr netmask;
	u16 hello_intv;
	u8 option;
	u8 prio;
	u32 router_dis_intv;
	struct iaddr router_ip;
	struct iaddr bak_router_ip;	
};

struct s_bgp_h
{
	u32 iden1;
	u32 iden2;
	u32 iden3;
	u32 iden4;
	u16 length;
	u8 type;
};

#endif
