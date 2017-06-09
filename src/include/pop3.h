//===========================================================================
// pop3.h
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
#ifndef	_X_POP3_H_
#define	_X_POP3_H_

//-------------------------------------------------------------------------
// pop3 status
//-------------------------------------------------------------------------
#define	POP3_NR_STATUS		25

#define	POP3_STATE_END		-1	// ERROR

#define	POP3_STATE1		0	// INIT
#define	POP3_STATE2		1	// TCP_ESTABLISH
#define	POP3_STATE3		2	// SEND_USER
#define	POP3_STATE4		3	// RECV_USER
#define	POP3_STATE5		4	// SEND_USER_OK
#define	POP3_STATE6		5	// WAIT_PASS
#define	POP3_STATE7		6	// SEND_PASS
#define	POP3_STATE8		7	// RECV_PASS
#define	POP3_STATE9		8	// SEND_PASS_OK

#define	POP3_STATE10		9	// WAIT_MAIL_INFORM
#define	POP3_STATE11		10	// SEND_MAIL_INFORM
#define	POP3_STATE12		11	// RECV_MAIL_INFORM
#define	POP3_STATE13		12	// SEND_MAIL_INFORM_OK

#define	POP3_STATE14		13	// WAIT_MAIL_LIST
#define	POP3_STATE15		14	// SEND_MAIL_LIST
#define	POP3_STATE16		15	// RECV_MAIL_LIST
#define	POP3_STATE17		16	// SEND_MAIL_LIST_OK

#define	POP3_STATE18		17	// WAIT_RETR
#define	POP3_STATE19		18	// SEND_RETR
#define	POP3_STATE20		19	// RECV_RETR
#define	POP3_STATE21		20	// SEND_RETR_OK

#define	POP3_STATE22		21	// WAIT_QUIT
#define	POP3_STATE23		22	// SEND_QUIT
#define	POP3_STATE24		23	// RECV_QUIT
#define	POP3_STATE25		24	// CLOSE
//-------------------------------------------------------------------------
// pop3 action in each status
//-------------------------------------------------------------------------
#define	POP3_NR_ACTION		15

#define	EX_TIME			0	//

#define	SEND_USERNAME		1	// 
#define	SEND_PASSWORD		2	//
#define	SEND_MAILLIST		3	// 
#define	SEND_MAILINFORM		4	// 
#define	SEND_CMD_OK		5	//
#define	SEND_RETR		6	//
#define	SEND_QUIT		7	//


#define	RECV_USERNAME		8	// 
#define	RECV_PASSWORD		9	//
#define	RECV_MAILLIST		10	// 
#define	RECV_MAILINFORM		11	// 
#define	RECV_CMD_OK		12	//
#define	RECV_RETR		13	//
#define	RECV_QUIT		14	//

//-------------------------------------------------------------------------
// http header
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// pop3 cmd
//-------------------------------------------------------------------------

#define	NR_POP3_CMD		12		//
#define	POP3_CMD_SIZE		5		//

#define	CMD_USER		0		//
#define	CMD_PASS		1		//
#define	CMD_APOP		2		//
#define	CMD_STAT		3		//
#define	CMD_UIDL		4		//
#define	CMD_LIST		5		//
#define	CMD_RETR		6		//
#define	CMD_DELE		7		//
#define	CMD_RESET		8		//
#define	CMD_TOP			9		//
#define	CMD_NOOP		10		//
#define	CMD_QUIT		11		//


//-------------------------------------------------------------------------
// pop3 state code
//-------------------------------------------------------------------------
#define	NR_POP3_STATE			2		//
#define	POP3_STATE_SIZE			5		//

#define	STS_P_OK			0		// +OK
#define	STS_N_ERR			1		// -ERR

PUBLIC s32 	*p_pop3_state_tab;

PUBLIC s32 	pop3_state;

PUBLIC u8	*p_pop3_stack;

PUBLIC	s8*	p_pop3_cmd;

PUBLIC 	s8*	p_ppop3_state;

PUBLIC	s8	flg_pop3_client;

#endif
