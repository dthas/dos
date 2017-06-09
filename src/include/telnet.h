//===========================================================================
// telnet.h
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
#ifndef	_X_TELNET_H_
#define	_X_TELNET_H_

//-------------------------------------------------------------------------
// telnet cmd
//-------------------------------------------------------------------------
#define	NR_TELNET_CMD		3
#define	TELNET_CMD_SIZE		10

#define	TELNET_CMD_LS		0
#define	TELNET_CMD_QUIT		1
#define	TELNET_CMD_PWD		2


//-------------------------------------------------------------------------
// telnet status
//-------------------------------------------------------------------------
#define	TELNET_NR_STATUS	12

#define	TELNET_STATE_END	-1	// ERROR

#define	TELNET_STATE1		0	// INIT
#define	TELNET_STATE2		1	// TCP_ESTABLISH
#define	TELNET_STATE3		2	// SEND_USER_TITLE
#define	TELNET_STATE4		3	// RECV_USER_TITLE
#define	TELNET_STATE5		4	// SEND_USER_NAME
#define	TELNET_STATE6		5	// RECV_USER_NAME
#define	TELNET_STATE7		6	// SEND_PASS_TITLE
#define	TELNET_STATE8		7	// RECV_PASS_TITLE
#define	TELNET_STATE9		8	// READY

#define	TELNET_STATE10		9	// SEND_CMD
#define	TELNET_STATE11		10	// RECV_CMD
#define	TELNET_STATE12		11	// CLOSE

//-------------------------------------------------------------------------
// telnet action in each status
//-------------------------------------------------------------------------
#define	TELNET_NR_ACTION	17

#define	EX_TIME			0	//

#define	SEND_USERTITLE		1	// 
#define	SEND_USERNAME		2	//
#define	SEND_PASSTITLE		3	// 
#define	SEND_PASSWORD		4	// 
#define	SEND_CMD		5	//
#define	SEND_RES		6	//
#define	SEND_CTRL		7	//
#define	SEND_QUIT		8	//


#define	RECV_USERTITLE		9	// 
#define	RECV_USERNAME		10	//
#define	RECV_PASSTITLE		11	// 
#define	RECV_PASSWORD		12	// 
#define	RECV_CMD		13	//
#define	RECV_RES		14	//
#define	RECV_CTRL		15	//
#define	RECV_QUIT		16	//

//-------------------------------------------------------------------------
// http header
//-------------------------------------------------------------------------
struct s_ctrl_cmd_simp
{
	u8	p1;
	u8	p2;
	u8	p3;
};

struct s_ctrl_cmd_comp
{
	u8	p1;
	u8	p2;
	u8	p3;
	u8	p4;
	u8	p5;
	u8	p6
};

//-------------------------------------------------------------------------
// TELNET
//-------------------------------------------------------------------------

#define	NR_TELNET_OPT		8		//

#define	OPT_BINARY		0		//
#define	OPT_ECHO		1		//
#define	OPT_GOAHEAD		3		//
#define	OPT_STATUS		5		//
#define	OPT_TIMING_MARK		6		//
#define	OPT_TERM_TYPE		24		//
#define	OPT_WINDOW_SIZE		31		//
#define	OPT_TERM_SPEED		32		//
#define	OPT_REMOTE_FLOW		33		//
#define	OPT_LINEMODE		34		//
#define	OPT_XWINDOW		35		//
#define	OPT_NEWENV		39		//


//-------------------------------------------------------------------------
// telnet control code
//-------------------------------------------------------------------------
#define	NR_TELNET_CTRL		18		//

#define	CTRL_EOF		236		// 
#define	CTRL_EOR		239		// 
#define	CTRL_SE			240		// 
#define	CTRL_NOP		241		// 
#define	CTRL_DM			242		// 
#define	CTRL_BRK		243		// 
#define	CTRL_IP			244		// 
#define	CTRL_AO			245		// 
#define	CTRL_AYT		246		// 
#define	CTRL_EC			247		// 
#define	CTRL_EL			248		// 
#define	CTRL_GA			249		// 
#define	CTRL_SB			250		// 
#define	CTRL_WILL		251		// 
#define	CTRL_WONT		252		// 
#define	CTRL_DO			253		// 
#define	CTRL_DONT		254		// 
#define	CTRL_IAC		255		// 


PUBLIC s32 	telnet_state;

PUBLIC 	s8*	p_telnet_state;

PUBLIC u8	*p_telnet_stack;

PUBLIC	s8	flg_telnet_client;

PUBLIC s32 	*p_telnet_state_tab;

PUBLIC	s8*	p_telnet_cmd;

#endif
