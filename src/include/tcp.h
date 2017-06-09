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
#ifndef	_X_TCP_H_
#define	_X_TCP_H_

//-------------------------------------------------------------------------
// tcp header
//-------------------------------------------------------------------------

#define FIN_BIT				0x1
#define SYN_BIT				0x2
#define PST_BIT				0x4
#define PSH_BIT				0x8
#define ACK_BIT				0x10
#define URG_BIT				0x20

#define TCP_HEADER_LENGTH		20
#define TCP_HEADER_LENGTH_IN_DWORD	5		//5*4=20
#define PTCP_HEADER_LENGTH		12

#define TCP_WINSIZE_DEFAULT		2048

#define TCP_CLIENT_PORT_DEFAULT		51791

#define TCP_OPTION_MSS_TYPE	2
#define TCP_OPTION_MSS_LEN	4
#define TCP_OPTION_MSS_SEG_MAX	1460

#define TCP_OPTION_WIN_TYPE	3
#define TCP_OPTION_WIN_LEN	3
#define TCP_OPTION_WIN_INDEX	6

#define TCP_OPTION_TIME_TYPE	8
#define TCP_OPTION_TIME_LEN	10

#define TCP_OPTION_SACK_TYPE	4
#define TCP_OPTION_SACK_LEN	2

#define TCP_OPTION_NOP		1

struct s_tcp_header
{
	u16 src_port;
	u16 dst_port;
	u32 seq;
	u32 ack;
	u8  header_len;
	u8  ctrl_bit;
	u16 winsize;
	u16 checksum;
	u16 upointer;		
};

struct s_ptcp_header
{
	struct iaddr src_ip;
	struct iaddr dst_ip;
	u8 zero;
	u8 protocol;
	u16 tcp_len;	
};

struct s_g_tcp_header
{
	struct s_ptcp_header p;
	struct s_tcp_header  t;
};

struct s_tcp_option_mss
{
	u8 type;
	u8 len;
	u16 max_seg_len;
};

struct s_tcp_option_win
{
	u8 type;
	u8 len;
	u8 win_index;
};

struct s_tcp_option_time
{
	u8 type;
	u8 len;
	u32 cur_time;
	u32 reply_time;
};

struct s_tcp_option_sack
{
	u8 type;
	u8 len;
};

PUBLIC u16	client_port;

//-------------------------------------------------------------------------
// tcp status
//-------------------------------------------------------------------------
#define	NR_STATUS	14

#define	TCP_STATE_END	-1	// ERROR

#define	TCP_STATE1		0	// CLOSE
#define	TCP_STATE2		1	// LISTEN
#define	TCP_STATE3		2	// SYN_SENT
#define	TCP_STATE4		3	// SYN_RCVD
#define	TCP_STATE5		4	// ESTABLISHED
#define	TCP_STATE6		5	// FIN-WAIT-1
#define	TCP_STATE7		6	// FIN-WAIT-2
#define	TCP_STATE8		7	// TIME-WAIT
#define	TCP_STATE9		8	// CLOSE-WAIT
#define	TCP_STATE10		9	// LAST-ACK
#define	TCP_STATE11		10	// ETABLISHED-1
#define	TCP_STATE12		11	// SYN_RCVD-1
#define	TCP_STATE13		12	// TIME-WAIT-1
#define	TCP_STATE14		13	// CLOSE-WAIT-1

//-------------------------------------------------------------------------
// tcp action in each status
//-------------------------------------------------------------------------
#define	NR_ACTION	13

#define	EX_TIME		0	//

#define	SEND_SYN	1	//
#define	SEND_ACK	2	//
#define	SEND_FIN	3	//
#define	SEND_SYN_ACK	4	//
#define	SEND_FIN_ACK	5	//

#define	RECV_SYN	6	//
#define	RECV_ACK	7	//
#define	RECV_FIN	8	//
#define	RECV_SYN_ACK	9	//
#define	RECV_FIN_ACK	10	//

#define	SEND_DATA	11	//
#define	RECV_DATA	12	//

PUBLIC s32 * p_tcp_state_tab;

PUBLIC s32 	tcp_state;

PUBLIC u32	seq, ack;

PUBLIC u32	connect_seq;		//the seq while connecting establish
PUBLIC u32	connect_ack;		//the ack while connecting establish

#endif
