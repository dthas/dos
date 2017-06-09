//===========================================================================
// smtp.h
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
#ifndef	_X_SMTP_H_
#define	_X_SMTP_H_

//-------------------------------------------------------------------------
// smtp status
//-------------------------------------------------------------------------
#define	SMTP_NR_STATUS		24

#define	SMTP_STATE_END		-1	// ERROR

#define	SMTP_STATE1		0	// INIT
#define	SMTP_STATE2		1	// TCP_ESTABLISH
#define	SMTP_STATE3		2	// PRE_ESTABLISH(SMTP)
#define	SMTP_STATE4		3	// READY_CLIENT
#define	SMTP_STATE5		4	// READY_SERVER
#define	SMTP_STATE6		5	// MAIL_ESTABLISH
#define	SMTP_STATE7		6	// SEND_FROM
#define	SMTP_STATE8		7	// RECV_FROM
#define	SMTP_STATE9		8	// SEND_FROM_OK
#define	SMTP_STATE10		9	// WAIT_RCPT
#define	SMTP_STATE11		10	// SEND_RCPT
#define	SMTP_STATE12		11	// RECV_RCPT
#define	SMTP_STATE13		12	// SEND_RCPT_OK
#define	SMTP_STATE14		13	// WAIT_DATA
#define	SMTP_STATE15		14	// PRE_SEND_DATA
#define	SMTP_STATE16		15	// RECV_DATA_READY
#define	SMTP_STATE17		16	// SEND_DATA
#define	SMTP_STATE18		17	// RECV_DATA
#define	SMTP_STATE19		18	// READY_CLOSE_CLIENT
#define	SMTP_STATE20		19	// READY_CLOSE_SERVER
#define	SMTP_STATE21		20	// WAIT_CLOSE_CLIENT
#define	SMTP_STATE22		21	// WAIT_CLOSE_SERVER
#define	SMTP_STATE23		22	// RECV_QUIT
#define	SMTP_STATE24		23	// CLOSE

//-------------------------------------------------------------------------
// smtp action in each status
//-------------------------------------------------------------------------
#define	SMTP_NR_ACTION		20

#define	EX_TIME			0	//

#define	SEND_SERVERREADY	1	// 220
#define	SEND_HELO		2	//
#define	SEND_OK			3	// 250
#define	SEND_MAILFROM		4	// 
#define	SEND_RCPTTO		5	//
#define	SEND_DATA		6	//
#define	SEND_STARTMAILREADY	7	// 354
#define	SEND_QUIT		8	//
#define	SEND_CLOSE		9	// 221

#define	RECV_SERVERREADY	10	// 220
#define	RECV_HELO		11	//
#define	RECV_OK			12	// 250
#define	RECV_MAILFROM		13	// 
#define	RECV_RCPTTO		14	//
#define	RECV_DATA		15	//
#define	RECV_STARTMAILREADY	16	// 354
#define	RECV_QUIT		17	//
#define	RECV_CLOSE		18	// 221

#define	RECV_DOT		19	// .

//-------------------------------------------------------------------------
// http header
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// smtp cmd
//-------------------------------------------------------------------------

#define	NR_CMD			15		//
#define	CMD_SIZE		10		//

#define	CMD_EHLO		0		//
#define	CMD_MAILFROM		1		//
#define	CMD_RCPTTO		2		//
#define	CMD_DATA		3		//
#define	CMD_QUIT		4		//
#define	CMD_RSET		5		//
#define	CMD_VRFY		6		//
#define	CMD_NOOP		7		//
#define	CMD_TURN		8		//
#define	CMD_EXPN		9		//
#define	CMD_HELP		10		//
#define	CMD_SENDFROM		11		//
#define	CMD_SMOLFROM		12		//
#define	CMD_SMALFROM		13		//
#define	CMD_RECEIVEMAIL		14		//

//-------------------------------------------------------------------------
// http state code
//-------------------------------------------------------------------------
#define	NR_SMTP_STATE			21		//
#define	SMTP_STATE_SIZE			4		//

#define	STS_SERVICE_STATE		0		//211
#define	STS_HELP			1		//214
#define	STS_SERVICE_READY		2		//220
#define	STS_SERVICE_CLOSE_TRANS		3		//221
#define	STS_CMD_OK			4		//250
#define	STS_FORWARD			5		//251
#define	STS_START_MAIL_INPUT		6		//354
#define	STS_SERVICE_DISABLE		7		//421
#define	STS_MAILBOX_DISABLE		8		//450
#define	STS_EXCEPTION_LOCAL		9		//451
#define	STS_EXCEPTION_MEMORY		10		//452
#define	STS_WRONG_GRAMMAR		11		//500
#define	STS_WRONG_PARAM			12		//501
#define	STS_CMD_CANT_EXE		13		//502
#define	STS_CMD_SEQ_WRONG		14		//503
#define	STS_CMD_CANT_EXE_NOW		15		//504
#define	STS_CMD_NOT_EXE			16		//550
#define	STS_USER_NOT_LOCAL		17		//551
#define	STS_ACTION_EXCEPTION		18		//552
#define	STS_ACTION_NOT_HAPPEN		19		//553
#define	STS_TRANS_FAILED		20		//554

PUBLIC s32 	*p_smtp_state_tab;

PUBLIC s32 	smtp_state;

PUBLIC u8	*p_smtp_stack;

PUBLIC	s8*	p_smtp_cmd;

PUBLIC	s8*	p_server_smtp_cmd;

PUBLIC 	s8*	p_ssmtp_state;

PUBLIC	s8	flg_client;

PUBLIC	s8*	p_mailname;

#endif
