//===========================================================================
// snmp.h
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
#ifndef	_X_SNMP_H_
#define	_X_SNMP_H_

//-------------------------------------------------------------------------
// snmp status
//-------------------------------------------------------------------------
#define	SNMP_NR_STATUS	4

#define	SNMP_STATE_END	-1		// ERROR

#define	SNMP_STATE1		0	// INIT
#define	SNMP_STATE2		1	// WAIT
#define	SNMP_STATE3		2	// READY
#define	SNMP_STATE4		3	// END

//-------------------------------------------------------------------------
// snmp action in each status
//-------------------------------------------------------------------------
#define	SNMP_NR_ACTION	15

#define	EX_TIME			0	//

#define	SEND_GETREQUEST		1	//
#define	SEND_GETNEXTREQUEST	2	//
#define	SEND_GETBULKREQUEST	3	//
#define	SEND_SETREQUEST		4	//
#define	SEND_RESPONSE		5	//
#define	SEND_TRAP		6	//
#define	SEND_REPORT		7	//
#define	SEND_INFORMREQUEST	8	//

#define	RECV_GETREQUEST		9	//
#define	RECV_GETNEXTREQUEST	10	//
#define	RECV_GETBULKREQUEST	11	//
#define	RECV_SETREQUEST		12	//
#define	RECV_RESPONSE		13	//
#define	RECV_TRAP		14	//

//-------------------------------------------------------------------------
// pdu type
//-------------------------------------------------------------------------
#define	GETREQUEST		0xA0	//
#define	GETNEXTREQUEST		0xA1	//
#define	RESPONSE		0xA2	//
#define	SETREQUEST		0xA3	//

#define	GETBULKREQUEST		0xA5	//
#define	INFORMREQUEST		0xA6	//
#define	TRAP			0xA7	//
#define	REPORT			0xA8	//

//-------------------------------------------------------------------------
// data type
//-------------------------------------------------------------------------
#define	INTEGER			0x2	//
#define	OCTET_STRING		0x4	//
#define	OBJECT_IDENTIFIER	0x6	//
#define	NULL			0x5	//
#define	SEQUENCE		0x30	//
#define	IPADDRESS		0x40	//
#define	COUNTER			0x41	//
#define	GAUGE			0x42	//
#define	TIMETICKS		0x43	//
#define	OPAQUE			0x44	//

PUBLIC s32 	*p_snmp_state_tab;

PUBLIC s32 	snmp_state;

PUBLIC u8	*p_snmp_stack;

#endif
