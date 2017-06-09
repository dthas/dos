//===========================================================================
// dns.h
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
#ifndef	_X_DNS_H_
#define	_X_DNS_H_

#define	DNS_HEADER_LENGTH	12

//-------------------------------------------------------------------------
// dns header
//-------------------------------------------------------------------------

struct s_dns_header
{
	u16 id;
	u16 flag;
	u16 question_cnt;
	u16 answer_cnt;
	u16 auth_cnt;
	u16 addition_cnt;			
};

// flag
#define	QR		0x8000
#define	OPCODE		0x7800
#define	AA		0x400
#define	TC		0x200
#define	RD		0x100
#define	RA		0x80
#define	RCODE		0xF

//opcode
#define	OP_STANDER	0
#define	OP_REVERSE	1
#define	OP_STATUS	2

//question type
#define	QTYPE_A		1		// NAME -> IP
#define	QTYPE_PTR	12		// IP   -> NAME

//question class
#define	QCLASS_AN	1		// internet

//-------------------------------------------------------------------------
// dns status
//-------------------------------------------------------------------------
#define	DNS_NR_STATUS	3

#define	DNS_STATE_END	-1		// ERROR

#define	DNS_STATE1		0	// CLOSE
#define	DNS_STATE2		1	// WAIT_ANSWER
#define	DNS_STATE3		2	// PROCESSING

//-------------------------------------------------------------------------
// dns action in each status
//-------------------------------------------------------------------------
#define	DNS_NR_ACTION	5

#define	EX_TIME		0	//

#define	SEND_IP		1	//
#define	SEND_NAME	2	//
#define	RECV_IP		3	//
#define	RECV_NAME	4	//

PUBLIC s32 	*p_dns_state_tab;

PUBLIC s32 	dns_state;

PUBLIC u8	*p_dns_stack;

PUBLIC s8	*p_domain_name;
PUBLIC s8	*p_domain_name_format;
PUBLIC s8	*p_ip_format;

#endif
