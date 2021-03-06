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

extern	u32 	ticket;

PRIVATE	s8	s_format[2][10] = {"netascii","octet"};

//===========================================================================
// tftp_add_rrq
//===========================================================================
PUBLIC	s32 	tftp_add_rrq(struct iaddr dst_ip, struct s_package *dst_pkg, u16 optcode, s8 *filename, s8 i_format)
{
	u8 src_action	= SEND_RRQ;
	tftp_state	= *(p_tftp_state_tab + tftp_state * TFTP_NR_ACTION + src_action);	

	s8* dst, src;
	s16 fname_len, format_len;

	s8* tftp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s16 total_len		= 0;
	u16 option_len		= 0;
	//------------------------------------------------------------------
	// optcode
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(optcode);

	total_len		= OPTCODE_LEN;		//2

	//------------------------------------------------------------------
	// file name
	//------------------------------------------------------------------
	fname_len		= strlen(filename);
	strcpy(&tftp[total_len], filename, fname_len);			//copy the filename

	total_len		+= fname_len;		// 2 + fname_len 

	//------------------------------------------------------------------
	// zero(1 byte)
	//------------------------------------------------------------------
	tftp[total_len]		= NULL;

	total_len		+= 1;			// 2 + fname_len + 1

	//------------------------------------------------------------------
	// format
	//------------------------------------------------------------------
	format_len		= strlen(&s_format[i_format][0]);
	strcpy(&tftp[total_len], &s_format[i_format][0], format_len);			//copy the format

	total_len		+= format_len;		// 2 + fname_len + 1 + format_len

	//------------------------------------------------------------------
	// zero(1 byte)
	//------------------------------------------------------------------
	tftp[total_len]		= NULL;

	total_len++;

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= get_port();
	s16 dst_port	= PORT_TFTP;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}


//===========================================================================
// tftp_add_wrq
//===========================================================================
PUBLIC	s32 	tftp_add_wrq(struct iaddr dst_ip, struct s_package *dst_pkg, u16 optcode, s8 *filename, s8 i_format)
{
	u8 src_action	= SEND_WRQ;
	tftp_state	= *(p_tftp_state_tab + tftp_state * TFTP_NR_ACTION + src_action);
	
	s8* dst, src;
	s16 fname_len, format_len;

	s8* tftp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s16 total_len		= 0;
	u16 option_len		= 0;
	//------------------------------------------------------------------
	// optcode
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(optcode);

	total_len		= OPTCODE_LEN;		//2

	//------------------------------------------------------------------
	// file name
	//------------------------------------------------------------------
	fname_len		= strlen(filename);
	strcpy(&tftp[total_len], filename, fname_len);			//copy the filename

	total_len		+= fname_len;		// 2 + fname_len 

	//------------------------------------------------------------------
	// zero(1 byte)
	//------------------------------------------------------------------
	tftp[total_len]		= NULL;

	total_len		+= 1;			// 2 + fname_len + 1

	//------------------------------------------------------------------
	// format
	//------------------------------------------------------------------
	format_len		= strlen(&s_format[i_format][0]);
	strcpy(&tftp[total_len], &s_format[i_format][0], format_len);			//copy the format

	total_len		+= format_len;		// 2 + fname_len + 1 + format_len

	//------------------------------------------------------------------
	// zero(1 byte)
	//------------------------------------------------------------------
	tftp[total_len]		= NULL;

	total_len++;	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= get_port();
	s16 dst_port	= PORT_TFTP;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}


//===========================================================================
// tftp_add_data
//===========================================================================
PUBLIC	s32 	tftp_add_data(struct s_package *src_pkg, struct s_package *dst_pkg, u16 optcode, s8 *data, s16 block_num)
{
	u8 src_action	= SEND_DATA;
	tftp_state	= *(p_tftp_state_tab + tftp_state * TFTP_NR_ACTION + src_action);

	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct iaddr src_ip	= iph->dst_ip;
	struct iaddr dst_ip	= iph->src_ip;

	struct s_udp_header *udph 	= (struct s_udp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	s8* dst, src;
	s16 data_len;

	s8* tftp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s16 total_len		= 0;
	u16 option_len		= 0;
	//------------------------------------------------------------------
	// optcode
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(optcode);

	total_len		= OPTCODE_LEN;		//2

	//------------------------------------------------------------------
	// block_num
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(block_num);

	total_len		+= BLOCKNUM_LEN;	// 2 + 2 

	//------------------------------------------------------------------
	// data
	//------------------------------------------------------------------
	
	data_len		= strlen(data);

	if(data_len < TFTP_DATA_BLOCK_SIZE)
	{
		strcpy(&tftp[total_len], data, data_len);

		total_len	+= data_len;			// 2 + 2 + data_len
	}
	else
	{
		strcpy(&tftp[total_len], data, TFTP_DATA_BLOCK_SIZE);

		total_len	+= TFTP_DATA_BLOCK_SIZE;	// 2 + 2 + 512
	}	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= big_little_16(udph->dst_port);
	s16 dst_port	= big_little_16(udph->src_port);

	add_udp_header(dst_pkg, src_ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, src_ip, dst_ip, PROTOCOL_UDP, total_len);
}


//===========================================================================
// tftp_add_ack
//===========================================================================
PUBLIC	s32 	tftp_add_ack(struct s_package *src_pkg, struct s_package *dst_pkg, u16 optcode, s16 block_num)
{
	u8 src_action	= SEND_ACK;
	tftp_state	= *(p_tftp_state_tab + tftp_state * TFTP_NR_ACTION + src_action);

	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct iaddr src_ip	= iph->dst_ip;
	struct iaddr dst_ip	= iph->src_ip;

	struct s_udp_header *udph 	= (struct s_udp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	s8* dst, src;
	s16 data_len;

	s8* tftp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s16 total_len		= 0;
	u16 option_len		= 0;
	//------------------------------------------------------------------
	// optcode
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(optcode);

	total_len		= OPTCODE_LEN;		//2

	//------------------------------------------------------------------
	// block_num
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(block_num);

	total_len		+= BLOCKNUM_LEN;	// 2 + 2 	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= big_little_16(udph->dst_port);
	s16 dst_port	= big_little_16(udph->src_port);

	add_udp_header(dst_pkg, src_ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, src_ip, dst_ip, PROTOCOL_UDP, total_len);
}



//===========================================================================
// tftp_add_error
//===========================================================================
PUBLIC	s32 	tftp_add_error(struct s_package *src_pkg, struct s_package *dst_pkg, u16 optcode, s8 *error_data, s16 err_num)
{
	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct iaddr src_ip	= iph->dst_ip;
	struct iaddr dst_ip	= iph->src_ip;

	struct s_udp_header *udph 	= (struct s_udp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	s8* dst, src;
	s16 err_len;

	s8* tftp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);
	s16 total_len		= 0;
	u16 option_len		= 0;
	//------------------------------------------------------------------
	// optcode
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(optcode);

	total_len		= OPTCODE_LEN;		//2

	//------------------------------------------------------------------
	// err num
	//------------------------------------------------------------------
	*(s16*)&tftp[total_len]	= little_big_16(err_num);

	total_len		+= ERRORNUM_LEN;	//2

	//------------------------------------------------------------------
	// error data
	//------------------------------------------------------------------
	dst			= &tftp[total_len];
	src			= error_data;
	err_len			= strlen(src);
	strcpy(dst, src, err_len);			//copy the format

	total_len		+= err_len;		// 2 + fname_len + 1 + format_len

	//------------------------------------------------------------------
	// zero(1 byte)
	//------------------------------------------------------------------
	tftp[total_len]		= NULL;

	total_len++;	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= big_little_16(udph->dst_port);
	s16 dst_port	= big_little_16(udph->src_port);

	add_udp_header(dst_pkg, src_ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, src_ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// rand_val
//===========================================================================
PUBLIC	u32	get_port(void)
{
	u32 port = ticket & 0xFFFF;

	return port;
}	
