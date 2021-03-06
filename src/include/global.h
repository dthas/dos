//===========================================================================
// global.h
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
#ifndef	_X_GLOBAL_H_
#define	_X_GLOBAL_H_

#define	TRUE	1
#define	FALSE	0

#define	UTC_BEIJING	8		//UTC time - beijing time = 8

#define	MEM_SEG_PAGE	100
#define	MEM_PAGE	101

#define	DESC_SYS_FIRST	0x28

#define	PROC_MSG	0
#define	PROC_TTY	1
#define	PROC_HD		2
#define	PROC_FS		3
#define	PROC_SYS	4
#define	PROC_UPROC	5
#define	PROC_NET	6
#define	PROC_FTP	7
#define	PROC_TCP_DAEMON	8
#define	PROC_TFTP	9
#define	PROC_DNS	10
#define	PROC_DHCP	11
#define	PROC_SNMP	12
#define	PROC_HTTP	13
#define	PROC_SMTP	14
#define	PROC_POP3	15
#define	PROC_TELNET	16


#define	MSG_NEW		1000
#define	MSG_EXPIRED	1001

#define	PG_N_EXIST	0	//page not exist bit
#define	PG_EXIST	1	//page exist bit
#define	PG_RW_W		2	//read/write bit
#define	PG_US_SYS	0	//user/system bit, this is for system
#define	PG_US_USER	4	//user/system bit, this is for user

#define	PAGE_SIZE	4096

#define	PG_REC_SIZE	131072	// total mem size space is 4G. each page is 4k. so, all the bits that need to record
				// the status of each page(present or not present) is 4G / 4K = 1M. That means the 
				// bytes that need to be used is 1M / 8bits/bytes = 131072(0x20000)


#define	KERNEL_PAGE_DIR_BASE	0x200000	//2MB
#define	KERNEL_PAGE_TABLE_BASE	0x201000	//2.004MB

//0xA00000 ~ 0xA01000	(4KB)		: page dir
//0xA01000 ~ 0xE01000	(4MB)		: page table
//0xE01000 ~ 0xE21000	(128kB)		: pages status
//0xFFFFF0 ~ 0x1000000	(16bytes)	: MEM_SIZE_ADDR, which is written in memsize32.asm
//0x1000000~ 		(4G)		: user program

#define	USER_PAGE_DIR_SIZE		0x1000		//4kb     (0   ~ 4kb)
#define	USER_PAGE_TABLE_SIZE		0x400000	//4mb     (4kb ~ 4.004MB)
#define	USER_PAGES_SIZE			0x20000		//128kb   (4.004MB  ~ 4.1289MB)
#define	USER_PAGE_VMEM_SIZE		0x800		//2kb     (4.1289MB ~ 4.1308MB) (1MB/4KB)*sizeof(struct s_vmem_map) 

#define	USER_PROC_ADDR		0x1000000	//16MB

#define	USER_PROC_PADDR		0x5FFFFC	//stores the physical start address of user process

#define	CMOS_INFO_ADDR		0x500


//for file read/write/append/execute
#define	FILE_MODE_R	1
#define	FILE_MODE_W	2
#define	FILE_MODE_X	3
#define	FILE_MODE_RW	4
#define	FILE_MODE_RX	5
#define	FILE_MODE_WX	6
#define	FILE_MODE_RWX	7
#define	FILE_MODE_A	8	

//for file attribute list(ls)
#define	LIST_MODE_L	1

#define FILE_NAME_LEN	64
#define DIR_NAME_LEN	64

enum tty_msg_type 
{
	TTY_READ = 20, TTY_WRITE, RESET_CMDL
};

enum sys_msg_type 
{
	SLEEP = 40, WAKEUP, CHANGE_WORKDIR, PRINT_DIR
};

enum proc_status_type 
{
	PROC_READY = 60, PROC_SLEEP
};

enum proc_flags_type 
{
	NO_MSG = 80, HAVE_MSG
};

enum syscall_type 
{
	MSG_SEND = 100, MSG_FORWARD, GET_PROC_FLAGS
};

enum msg_func 
{
	SYN = 120, ASYN
};

enum msg_blcok 
{
	NO_BLOCK = 140, BLOCKING
};

enum hd_msg_type 
{
	HARDDISK_READ = 160, HARDDISK_WRITE, HD_INITFREELIST, HD_INT
};

enum fs_msg_type 
{
	FS_OPEN = 180, FS_READ, FS_MKDIR, FS_MKFILE, FS_RMDIR, FS_RMFILE, FS_LS, FS_CAT, FS_CD, FS_PWD
};

enum uproc_msg_type 
{
	CMD_EXEC = 200, M_ALLOC, EXIT, ENPAGE, VM_ALLOC
};

enum net_msg_type 
{
	NET_SEND = 220, ICMP_0_8, ICMP_13_14, NETSTAT, IFCONFIG, PKG_PROCESS, TRACEROUTE, DY_RIP1, DY_RIP2, DY_OSPF2, DY_BGP, IGMP3
};


enum ftp_msg_type 
{
	FTP_CTRL = 260, FTP_DATA, FTP_OPEN, FTP_QUIT, FTP_USER, FTP_PASSWD, FTP_LS, FTP_GET, FTP_PUT, FTP_TYPE_I
};

enum tcp_msg_type 
{
	TCP_SEND = 280
};

enum tftp_msg_type 
{
	TFTP_PROCESS = 300, TFTP_READ, TFTP_WRITE
};

enum dns_msg_type 
{
	DNS_PROCESS = 320, DNS_IP_2_NAME, DNS_NAME_2_IP
};

enum dhcp_msg_type 
{
	DHCP_PROCESS = 340, DHCP_DISCOVER, DHCP_REQUEST, DHCP_RELEASE
};

enum snmp_msg_type 
{
	SNMP_PROCESS = 360, SNMP_GETREQUEST
};

enum http_msg_type 
{
	HTTP_PROCESS = 380, HTTP_GET, HTTP_OPEN
};

enum smtp_msg_type 
{
	SMTP_PROCESS = 400, SMTP_SEND, SMTP_OPEN
};

enum pop3_msg_type 
{
	POP3_PROCESS = 420, POP3_USER, POP3_OPEN
};

enum telnet_msg_type 
{
	TELNET_PROCESS = 440, TELNET_OPEN
};

#define	STACK_SIZE	0x2000 		//8 KB 

//===========================================================
// cmos info		: 0x500    ~ 0x540   (64bytes)
// kernel		: 0x100000 ~ 0x200000(1MB ~ 2MB)
// kernel page		: 0x200000 ~ 0x700000(2MB ~ 7MB)
// p_tmp_data 		: 0x700000 ~ 0x800000(7MB ~ 8MB)
// p_send_nbuf		: 0x800000 ~ 0x880000(8MB ~ 8.5MB)
// p_recv_nbuf		: 0x880000 ~ 0x900000(8.5MB ~ 9MB)
// p_temp_nbuf		: 0x900000 ~ 0x980000(9MB ~ 9.5MB)
// --------------------------------------------------------
//
// user program start   : 0xA00000				// (offset 0)
// user page dir	: 0xA00000 ~ 0xA01000	(4KB)		
// user page table	: 0xA01000 ~ 0xE01000	(4MB)		 
// user pages status	: 0xE01000 ~ 0xE21000	(128kB)	
// CMOS(sys date0)	: 0xFFFFA0 ~ 0xFFFFE0   (64bytes)       // which is written in cmos_info32.asm(the cmos info is from cmos_info16.asm)  	 
// MEM_SIZE_ADDR	: 0xFFFFF0 ~ 0x1000000	(16bytes)	// MEM_SIZE_ADDR, which is written in memsize32.asm
// user program		: 0x1000000~ 		(4G)		// (offset 6MB)code seg + data seg + stack seg 
//===========================================================

#define	VFS_TMP_DATA	0x700000 		//7MB 

#define	NET_SEND_BUF	0x800000 		//8MB   ~ 8.5MB (512KB)
#define	NET_RECV_BUF	0x880000 		//8.5MB ~ 9MB   (512KB)

#define	NET_TEMP_BUF	0x900000 		//8.5MB ~ 9MB   (512KB)

PUBLIC	s8	*p_tmp_data;

PUBLIC	s8	*p_send_nbuf;
PUBLIC	s8	*p_recv_nbuf;

#endif
