//===========================================================================
// type.h
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

#ifndef	_X_TYPE_H_
#define	_X_TYPE_H_

#define	PUBLIC		
#define	PRIVATE	static	

#define	NULL	'\0'

typedef	unsigned long long	u64;
typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;

typedef	long long		s64;
typedef	int			s32;
typedef	short			s16;
typedef	char			s8;

//just for vfs interface
typedef	void	(*fs_op)	(s32 dev);
typedef	s32	(*fs_op1)	(s32 dev, u8 * dir_name);
typedef	u32	(*fs_op2)	(s32 dev);
typedef	s32	(*fs_op4)	(s32 dev, u32 sec, s8 * filename);
typedef	void	(*fs_op5)	(s32 dev, s32 num);
typedef	void	(*fs_op6)	(s32 dev, s32 num1, u32 num2);
typedef	u32	(*fs_op7)	(s32 dev, u8 * dir_name, struct s_v_inode *src_inode);
typedef	u32	(*fs_op8)	(s32 dev, struct s_v_inode *src_inode);
typedef	struct s_v_super_blk*	(*fs_op3)	(s32 dev);

//for proc
typedef	void	(*proc_t)	(void);

//for syscall
typedef	void	(*syscall_t)	(void);

//for message(24 bytes)
struct 	t_mesg
{
	s32	parm1;
	s32	parm2;
	s32	parm3;
	u32	parm4;
	void*	buf1;
	void*	buf2;
};

struct	s_mesg
{
	s32	src_pid;
	s32	dest_pid;
	s32	type;
	s32	func;		// SYN, ASYN
	s32	flag;		//if the mesg has sent, then flag is EXPIRED

	struct	t_mesg	msg;
};

//for tcp/ip
struct hwaddr
{
	u8	addr1;
	u8	addr2;
	u8	addr3;
	u8	addr4;
	u8	addr5;
	u8	addr6;
};

struct iaddr
{
	u8	addr1;
	u8	addr2;
	u8	addr3;
	u8	addr4;
};

//just for http(add on 2013-3-15)
typedef	s32	(*http_op)	(struct iaddr dst_ip, struct s_package *src_pkg, struct s_package *dst_pkg);
typedef	s32	(*smtp_op)	(struct iaddr dst_ip, struct s_package *src_pkg, struct s_package *dst_pkg);
typedef	s32	(*pop3_op)	(struct iaddr dst_ip, struct s_package *src_pkg, struct s_package *dst_pkg);
typedef	s32	(*telnet_op)	(struct iaddr dst_ip, struct s_package *src_pkg, struct s_package *dst_pkg);

#endif
