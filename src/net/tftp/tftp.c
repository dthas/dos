//===========================================================================
// tftp.c
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

#include	"type.h"
#include	"stdio.h"
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"tty.h"
#include	"keyboard.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"rtl8029.h"
#include	"frame.h"
#include	"net.h"
#include	"route.h"
#include	"ipv4.h"
#include	"udp.h"
#include	"tftp.h"
#include	"prototype.h"

extern u32 	ticket;

PRIVATE	s8 	*tmp_tcp_data = NET_TEMP_BUF;		//0x9MB, define in global.h

PRIVATE	s32	tftp_state_tab[TFTP_NR_STATUS][TFTP_NR_ACTION];

PRIVATE	void	tftp_statetab_init();

PUBLIC	u8	tftp_stack[STACK_SIZE];

s8 dname[20]	= "tfdl.txt";
s8 uname[20]	= "tfup.txt";

//===========================================================================
// tftp main
//===========================================================================
PUBLIC	void 	tftp_main()
{
	mkdir("/test");
	mkfile("/test/tfup.txt");
	mkfile("/test/tfdl.txt");
	
	s32 i;
	for(i=0; i<400; i++)
	{
		p_send_nbuf[i]	= 'k';
	}
	p_send_nbuf[i]	= NULL;
	cat("/test/tfup.txt", p_send_nbuf,   FILE_MODE_W);
	
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_TFTP;
		mesg.dest_pid 	= PROC_TFTP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case TFTP_PROCESS:
					tftp_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case TFTP_READ:
					s2ip(&ip, mesg.msg.buf1);
					tftp_add_rrq(ip, &dst_pkg, OPT_RRQ, dname, NETASCII);
					break;	
				case TFTP_WRITE:
					s2ip(&ip, mesg.msg.buf1);
					tftp_add_wrq(ip, &dst_pkg, OPT_WRQ, uname, NETASCII);
					break;	
				default:
					break;
			}

			if(mesg.func == SYN)
			{
				if(mesg.type != EXIT)
				{
					struct s_mesg mesg1;

					mesg1.dest_pid	= mesg.src_pid;
					mesg1.type	= PROC_READY;
		
					setstatus(&mesg1);
				}
			}
		}	
	}			
}

//===========================================================================
// init tcp
//===========================================================================
PUBLIC	void	init_tftp()
{
	p_tftp_stack 	= tftp_stack;


	tftp_statetab_init();

	tftp_state	= TFTP_STATE1;

	p_send_nbuf	= NET_SEND_BUF;		//8MB   ~ 8.5MB (512KB) global.h
	p_recv_nbuf	= NET_RECV_BUF;		//8.5MB ~ 9MB   (512KB) global.h

//------for test
	char *str = "--tftp init--";
	disp_str(str);
//------for test

}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	tftp_statetab_init()
{
	s32 i,j;

	p_tftp_state_tab = tftp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<TFTP_NR_STATUS; i++)
	{
		for(j=0; j<TFTP_NR_ACTION; j++)
		{
			tftp_state_tab[i][j]	= TFTP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// TFTP_STATE1 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE1][SEND_RRQ]	= TFTP_STATE2;
	tftp_state_tab[TFTP_STATE1][SEND_WRQ]	= TFTP_STATE2;

	tftp_state_tab[TFTP_STATE1][RECV_RRQ]	= TFTP_STATE3;
	tftp_state_tab[TFTP_STATE1][RECV_WRQ]	= TFTP_STATE3;
	
	//------------------------------------------------------------------
	// TFTP_STATE2 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE2][RECV_DATA]	= TFTP_STATE4;
	tftp_state_tab[TFTP_STATE2][RECV_ACK]	= TFTP_STATE6;

	//------------------------------------------------------------------
	// TFTP_STATE3 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE3][SEND_DATA]	= TFTP_STATE5;
	tftp_state_tab[TFTP_STATE3][SEND_ACK]	= TFTP_STATE6;

	//------------------------------------------------------------------
	// TFTP_STATE4 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE4][SEND_ACK]	= TFTP_STATE6;

	//------------------------------------------------------------------
	// TFTP_STATE5 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE5][RECV_ACK]	= TFTP_STATE6;
	
	//------------------------------------------------------------------
	// TFTP_STATE6 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE6][RECV_DATA]	= TFTP_STATE7;
	tftp_state_tab[TFTP_STATE6][SEND_DATA]	= TFTP_STATE8;

	//------------------------------------------------------------------
	// TFTP_STATE7 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE7][SEND_ACK]	= TFTP_STATE6;	
	
	//------------------------------------------------------------------
	// TFTP_STATE8 
	//------------------------------------------------------------------
	tftp_state_tab[TFTP_STATE8][RECV_ACK]	= TFTP_STATE6;	

}


//===========================================================================
// tftp_process
//===========================================================================
PUBLIC	void 	tftp_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	s8* tftp_src		= &(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH +IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + OPTCODE_LEN + BLOCKNUM_LEN]));

	struct s_udp_header* udp= (struct s_udp_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	u16 udp_len		=  little_big_16(udp->udp_len);
	u16 recv_data_len	= udp_len - (UDP_HEADER_LENGTH + OPTCODE_LEN + BLOCKNUM_LEN);

	u8 src_action		= -1;
	u8 dst_action		= -1;

	u16	block_num	= 1;
	u16	ack		= 0;

	s8 	*data;
	s32	src_len;

	u16	src_optcode	= little_big_16(*(s16*)tftp_src);

	switch(src_optcode)
	{
		case	OPT_RRQ:
			src_action	= RECV_RRQ;
			dst_action	= SEND_DATA;
			break;
		case	OPT_WRQ:
			src_action	= RECV_WRQ;
			dst_action	= SEND_ACK;
			break;
		case	OPT_DAT:
			src_action	= RECV_DATA;
			dst_action	= SEND_ACK;

			block_num	= little_big_16(*(s16*)&tftp_src[OPTCODE_LEN]);
			ack		= block_num;

			if(recv_data_len < TFTP_DATA_BLOCK_SIZE)
			{				
				strcpy(p_recv_nbuf, src_d, recv_data_len);
				p_recv_nbuf		+= recv_data_len;

				*(p_recv_nbuf + 1)	= NULL;
				s8 fname[64] = "/test/tfdl.txt";
				cat(fname, (s8*)NET_RECV_BUF, FILE_MODE_W);
				p_recv_nbuf		= NET_RECV_BUF;
			}
			else
			{
				strcpy(p_recv_nbuf, src_d, TFTP_DATA_BLOCK_SIZE);	//exclude '\r\n'
				p_recv_nbuf		+= TFTP_DATA_BLOCK_SIZE;
			}
			
			break;

		case	OPT_ACK:
			src_action	= RECV_ACK;

			dst_action	= SEND_DATA;	//add on 2013-3-10

			ack		= little_big_16(*(s16*)&tftp_src[OPTCODE_LEN]);
			block_num	= ack + 1;

			break;
		case	OPT_ERR:
			src_action	= RECV_ERROR;
			break;
		default:
			break;
	}
	
	if(src_action)
	{
		tftp_state	= *(p_tftp_state_tab + tftp_state * TFTP_NR_ACTION + src_action);		
	}

	if(tftp_state && dst_action)
	{
		//-------------------------------------------------------------------------
		// add tcp option
		//-------------------------------------------------------------------------
		u16 option_len	= 0;

		//-------------------------------------------------------------------------
		// tftp
		//-------------------------------------------------------------------------
		u16 tftp_data_len = 0;

		switch(dst_action)
		{
			//------------------------------------------------------------------
			// send data
			//------------------------------------------------------------------
			case	SEND_DATA:
				if(block_num == 1)
				{	
					s8 fname[64] = "/test/tfup.txt";
					read(fname, FILE_MODE_R);			//read file data into 7MB
					src_len		= strlen((s8*)VFS_TMP_DATA);	
					strcpy((s8*)NET_SEND_BUF, (s8*)VFS_TMP_DATA, src_len); 	//copy data from 7MB to 8MB
				}

				data		= NET_SEND_BUF + (block_num - 1) * TFTP_DATA_BLOCK_SIZE;	//512
				
				tftp_add_data(src_pkg, dst_pkg, OPT_DAT, data, block_num);
				break;

			//------------------------------------------------------------------
			// send ack
			//------------------------------------------------------------------
			case	SEND_ACK:
				tftp_add_ack(src_pkg, dst_pkg, OPT_ACK, ack);
				break;
			default:
				break;
		}
	}
}
