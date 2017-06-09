//===========================================================================
// ftp.h
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
#ifndef	_X_FTP_H_
#define	_X_FTP_H_

PUBLIC u8	*p_ftp_stack;

PUBLIC u8	flg_ftp_user;
PUBLIC u8	flg_ftp_pass;
PUBLIC u8	flg_ftp_login;
PUBLIC u8	flg_ftp_quit;
PUBLIC u8	flg_ftp_syst;
PUBLIC u8	flg_ftp_port;
PUBLIC u8	flg_ftp_filests;
PUBLIC u8	flg_ftp_get;
PUBLIC u8	flg_ftp_put;
PUBLIC u8	flg_ftp_type;
PUBLIC u8	flg_ftp_close;
PUBLIC u8	flg_ftp_data_start;
PUBLIC u8	flg_ftp_data_end;
PUBLIC u8	flg_ftp_ls;

PUBLIC u16	ftp_control_port;	//using in data transfer, which save the control port
PUBLIC u8	ftp_control_state;	//using in data transfer, which save the previor state

PUBLIC u32	ftp_control_seq;	//using in data transfer, which save the previor seq 
PUBLIC u32	ftp_control_ack;	//using in data transfer, which save the previor ack

PUBLIC u16	ftp_data_port;

PUBLIC s8	*recv_buf_start_addr;
PUBLIC s8	*send_buf_start_addr;

PUBLIC u32	data_size_total_bytes;

//-------------------------------------------------------------------------
// FTP client data port
//-------------------------------------------------------------------------
#define	FTP_CLIENT_PORT	52131		//236 * 220 + 211

//-------------------------------------------------------------------------
// FTP status
//-------------------------------------------------------------------------
#define	SERVER_READY	0x323230
#define	SERVER_CLOSE	0x323231

#define	USER_LOGIN_OK	0x323330
#define	PASSWORD_NEED	0x333331
#define	ACCOUNT_NEED	0x333332
#define	TYPE_I		0x323030

#define	FILE_STATE_OK	0x313530
#define	DIR_OK		0x323236

//-------------------------------------------------------------------------
// FTP cmd
//-------------------------------------------------------------------------
#define	USER		0x55534552
#define	PASS		0x50415353
#define	QUIT		0x51554954
#define	SYST		0x53595354
#define	LIST		0x4C495354

#endif
