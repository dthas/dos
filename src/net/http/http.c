//===========================================================================
// http.c
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
#include	"tcp.h"
#include	"http.h"
#include	"prototype.h"

PRIVATE	s32	hhttp_state_tab[HTTP_NR_STATUS][HTTP_NR_ACTION];
PRIVATE	void	http_statetab_init();
PUBLIC	u8	http_stack[STACK_SIZE];

struct iaddr 	dest_ip;

http_op	http_fun;
//===========================================================================
// tftp main
//===========================================================================
PUBLIC	void 	http_main()
{
	while(1)
	{	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_HTTP;
		mesg.dest_pid 	= PROC_HTTP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case HTTP_PROCESS:
					http_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case HTTP_OPEN:
					s2ip(&ip, mesg.msg.buf1);
					http_open(ip, &dst_pkg);
					break;
				case HTTP_GET:
					s2ip(&ip, mesg.msg.buf1);
					http_fun= http_req_get;
					dest_ip	= ip;
					http_open(ip, &dst_pkg);
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
// init http
//===========================================================================
PUBLIC	void	init_http()
{
	p_hhttp_stack 	= http_stack;

	http_statetab_init();

	http_state	= HTTP_STATE1;

	parm_init();
	
//------for test
	char *str = "-init http-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	http_statetab_init()
{
	s32 i,j;

	p_hhttp_state_tab = hhttp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<HTTP_NR_STATUS; i++)
	{
		for(j=0; j<HTTP_NR_ACTION; j++)
		{
			hhttp_state_tab[i][j]	= HTTP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// HTTP_STATE1 
	//------------------------------------------------------------------
	hhttp_state_tab[HTTP_STATE1][SEND_HTTPREQUEST]	= HTTP_STATE2;
	hhttp_state_tab[HTTP_STATE1][RECV_HTTPREQUEST]	= HTTP_STATE2;

	//------------------------------------------------------------------
	// HTTP_STATE2 
	//------------------------------------------------------------------
	hhttp_state_tab[HTTP_STATE2][SEND_HTTPREQUEST]	= HTTP_STATE4;
	hhttp_state_tab[HTTP_STATE2][SEND_HTTPRESPONSE]	= HTTP_STATE3;

	//------------------------------------------------------------------
	// HTTP_STATE3 
	//------------------------------------------------------------------
	hhttp_state_tab[HTTP_STATE3][SEND_HTTPREQUEST]	= HTTP_STATE2;
	hhttp_state_tab[HTTP_STATE3][SEND_HTTPCLOSE]	= HTTP_STATE1;

	//------------------------------------------------------------------
	// HTTP_STATE4
	//------------------------------------------------------------------
	hhttp_state_tab[HTTP_STATE4][RECV_HTTPREQUEST]	= HTTP_STATE2;
	hhttp_state_tab[HTTP_STATE4][RECV_HTTPCLOSE]	= HTTP_STATE1;
}


//===========================================================================
// http_process
//===========================================================================
PUBLIC	void 	http_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;
		
	u16	dst_tcp_data_len	= 0;

	u8 option_len		= 0;
	
	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	
	if(src_tcp_data_len)
	{
		s8 t_http_version[10];
		s8 t_http_method[8];
		s8 t_http_url[64];
		struct s_state_code t_state_code;

		s8 flg_response = FALSE;
		s8 flg_tmp	= FALSE;
		s8 src[10];
		s8 dst[5] = "HTTP";
		s32 i,j,sts_code, method_id;
		s32 state_line_len = 0;
		u8 src_action;

		strcpy(src, src_d, 4);
		src[4]	= NULL;

		flg_response = strcmp(src, dst);

		if(flg_response)
		{
		//##################################################################
		// RECEIVE RESPONSE (client)
		//##################################################################
			i = 0;

			//------------------------------------------------------------------
			// 1)get http_version
			//------------------------------------------------------------------
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_http_version[j]	= *(src_d + i);
				}
			}

			
			//------------------------------------------------------------------
			//2)get state code.code
			//------------------------------------------------------------------
			i++;	//over the space
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_state_code.code[j]	= *(src_d + i);
				}
			}

			t_state_code.code[j]	= NULL;
			
			//------------------------------------------------------------------
			//3)get state code.phrase
			//------------------------------------------------------------------
			i++;	//over the space
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_state_code.phrase[j]	= *(src_d + i);
				}
			}
			
			t_state_code.phrase[j]	= NULL;

			//------------------------------------------------------------------
			//4)get state line length
			//------------------------------------------------------------------			
			state_line_len	= i + 2;	// over cr + lf

			//------------------------------------------------------------------
			//5)get sts_code
			//------------------------------------------------------------------
			for(i=0; i<NR_STS_CODE; i++)
			{
				if(flg_tmp = strcmp(t_state_code.code, (p_state_code+i)))
				{
					
					break;
				}
			}
			
			if(!flg_tmp)
			{
				printk("panic\n");

				while(1)
				{};
			}

			sts_code	= i;

			//------------------------------------------------------------------
			//6)action
			//------------------------------------------------------------------
			switch(sts_code)
			{
				case STS_OK:
					src_action	= SEND_HTTPRESPONSE;
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// http state change(change to HTTP_STATE3/HTTP_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			http_state	= *(p_hhttp_state_tab + http_state * HTTP_NR_ACTION + src_action);
			
		}
		else
		{
		//##################################################################
		// RECEIVE REQUEST (server)
		//##################################################################

			i = 0;

			//------------------------------------------------------------------
			// 1)get http_method
			//------------------------------------------------------------------
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_http_method[j]	= *(src_d + i);
				}
			}

			t_http_method[j]  = NULL;
			
			//------------------------------------------------------------------
			//2)get url
			//------------------------------------------------------------------
			i++;	//over the space
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_http_url[j]	= *(src_d + i);
				}
			}

			t_http_url[j]	= NULL;
			
			//------------------------------------------------------------------
			//3)get http_version
			//------------------------------------------------------------------
			i++;	//over the space
			for(j=0; i<512; i++,j++)
			{
				if(*(src_d+i) == 0x20)
				{
					break;
				}
				else
				{
					t_http_version[j]	= *(src_d + i);
				}
			}
			
			t_http_version[j]	= NULL;

			//------------------------------------------------------------------
			//4)get state line length
			//------------------------------------------------------------------			
			state_line_len	= i + 2;	// over cr + lf

			//------------------------------------------------------------------
			//5)get method
			//------------------------------------------------------------------
			for(i=0; i<NR_METHOD; i++)
			{
				if(flg_tmp = strcmp(t_http_method, (p_http_method+i)))
				{					
					break;
				}
			}
			
			if(!flg_tmp)
			{
				printk("panic\n");

				while(1)
				{};
			}

			method_id	= i;

			//------------------------------------------------------------------
			//6)action
			//------------------------------------------------------------------
			switch(method_id)
			{
				case METHOD_GET:				

					http_fun	= http_res_get;

					src_action	= RECV_HTTPREQUEST;
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// http state change(change to HTTP_STATE3/HTTP_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			http_state	= *(p_hhttp_state_tab + http_state * HTTP_NR_ACTION + src_action);
			
			http_fun(dest_ip, src_pkg, dst_pkg);

			return;
		}

			
	}
	
	if((http_state == HTTP_STATE1) && (tcp_state == TCP_STATE5))
	{		
		http_fun(dest_ip, src_pkg, dst_pkg);
		return;
	}

	if((http_state == HTTP_STATE3) && (tcp_state == TCP_STATE5))
	{
		http_close(src_pkg, dst_pkg);
		return;
	}

	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);
}
