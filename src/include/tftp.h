//===========================================================================
// tftp.h
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
#ifndef	_X_TFTP_H_
#define	_X_TFTP_H_

#define NETASCII			0
#define	OCTET				1

//-------------------------------------------------------------------------
// tcp header
//-------------------------------------------------------------------------

#define OPTCODE_LEN			2
#define BLOCKNUM_LEN			2
#define ERRORNUM_LEN			2

#define TFTP_DATA_BLOCK_SIZE		512

#define OPT_RRQ				0x1
#define OPT_WRQ				0x2
#define OPT_DAT				0x3
#define OPT_ACK				0x4
#define OPT_ERR				0x5

//-------------------------------------------------------------------------
// tftp status
//-------------------------------------------------------------------------
#define	TFTP_NR_STATUS	8

#define	TFTP_STATE_END	-1		// ERROR

#define	TFTP_STATE1		0	// CLOSE
#define	TFTP_STATE2		1	// CONNECTION_APP
#define	TFTP_STATE3		2	// CONNECTION_RECV
#define	TFTP_STATE4		3	// CONNECTION_ACK
#define	TFTP_STATE5		4	// CONNECTION_WAIT
#define	TFTP_STATE6		5	// ETABLISHED
#define	TFTP_STATE7		6	// READ_WRITE
#define	TFTP_STATE8		7	// WAIT_ACK

//-------------------------------------------------------------------------
// tftp action in each status
//-------------------------------------------------------------------------
#define	TFTP_NR_ACTION	11

#define	EX_TIME		0	//

#define	SEND_RRQ	1	//
#define	SEND_WRQ	2	//
#define	SEND_DATA	3	//
#define	SEND_ACK	4	//
#define	SEND_ERROR	5	//

#define	RECV_RRQ	6	//
#define	RECV_WRQ	7	//
#define	RECV_DATA	8	//
#define	RECV_ACK	9	//
#define	RECV_ERROR	10	//

PUBLIC s32 	*p_tftp_state_tab;

PUBLIC s32 	tftp_state;

PUBLIC u8	*p_tftp_stack;

#endif
