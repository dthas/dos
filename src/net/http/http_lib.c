//===========================================================================
// dhcp_lib.c
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

extern	u32	ticket;

PRIVATE	s8	http_version[10]		= "HTTP/1.1";

PRIVATE	s8	http_method[8][8]		= {"GET","HEAD","POST","PUT","TRACE","CONNECT","DELETE","OPTIONs"};

PRIVATE	s8	http_req_headername[11][20]		= {"User-Agent","Accept","Accept-Charset","Accept-Encoding","Accept-Language",
							"Authorization","Host","Date","Upgrade","Cookie","If-Modified-Since"};

PRIVATE	s8	http_res_headername[11][20]		= {"Date","Upgrade","Server","Set-Cookie","Content-Encoding","Content-Language",
							"Content-Length","Content-Type","Location","Accept-Ranges","Last-Modified"};

PRIVATE struct s_state_code state_code[NR_STS_CODE]	= {{"100", "Continue"}, {"101", "Switching"},
							{"200", "OK"}, {"201","Created"}, {"202", "Accepted"}, {"204", "No content"},
							{"301", "Moved permanently"}, {"302", "Moved temporarily"}, {"304", "Not modified"},
							{"400", "Bad request"}, {"401", "Unauthorized"}, {"403", "Forbidden"},
								{"404","Not found"}, {"405", "Method not allowed"}, {"406", "Not acceptable"},
							{"500","Internal server error"}, {"501", "Not implemented"},{"503","Service unavailable"}};

//for test
PRIVATE	s8	domain_name[20]			= "www.baidu.com";
PRIVATE	s8	server_name[20]			= "www.dthas.com";
PRIVATE	s8	content_type[10]		= "text/html";

PRIVATE s8	data[64]			= "<html><body><h1>dthas:hello world</body></html>\n";

PRIVATE	void	read_data();

//===========================================================================
// parm_init()
//===========================================================================
PUBLIC	void 	parm_init()
{
	p_state_code	= state_code;
	p_http_method	= http_method;
}

PRIVATE	void	read_data()
{
	s8 fname[64]	= "/test/index.html";	

	mkfile("/test/index.html");
	cat("/test/index.html", data,   FILE_MODE_W);

	read(fname, FILE_MODE_R);			//read file data into 7MB
	s32 str_len	= strlen((s8*)VFS_TMP_DATA);
	strcpy((s8*)NET_SEND_BUF, (s8*)VFS_TMP_DATA, str_len); 	//copy data from 7MB to 8MB
	*(s8*)(NET_SEND_BUF + str_len )	= NULL;
}

//===========================================================================
// http_get_request
//===========================================================================
PUBLIC	s32 	http_req_get(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	s8* http_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;
	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add method
	str_len	= strlen(&http_method[METHOD_GET][0]);
	strcpy(http_con, &http_method[METHOD_GET][0], str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;

	//3)add URL
	*http_con		= '/';		// /

	http_con		+= 1;
	total_len		+= 1;

	//4)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;

	//5)add version
	str_len	= strlen(http_version);
	strcpy(http_con, http_version, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//6)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;	

	//------------------------------------------------------------------
	// add header line(HOST)
	//------------------------------------------------------------------
	//1)add header name
	str_len	= strlen(&http_req_headername[REQ_HOST][0]);
	strcpy(http_con, &http_req_headername[REQ_HOST][0], str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add :
	*http_con		= 0x3a;		//:

	http_con		+= 1;
	total_len		+= 1;

	//3)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;	

	//4)add value
	str_len	= strlen(domain_name);
	strcpy(http_con, domain_name, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//5)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// NULL line
	//------------------------------------------------------------------
	//1)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// http state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_HTTPREQUEST;
	s32 old_state	= http_state;
	http_state	= *(p_hhttp_state_tab + http_state * HTTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	
	struct iaddr src_ip	= pnet_dev->ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_HTTP;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 	
	
	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// http_get_response
//===========================================================================
PUBLIC	s32 	http_res_get(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	s8* http_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;

	s8  tmp_data[10];
	//------------------------------------------------------------------
	// add status line
	//------------------------------------------------------------------
	//1)add version
	str_len	= strlen(http_version);
	strcpy(http_con, http_version, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;

	//3)add status code
	str_len	= strlen(&(state_code[STS_OK].code[0]));
	strcpy(http_con, &(state_code[STS_OK].code[0]), str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//4)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;

	//5)add status phrase
	str_len	= strlen(&(state_code[STS_OK].phrase[0]));
	strcpy(http_con, &(state_code[STS_OK].phrase[0]), str_len);

	http_con		+= str_len;
	total_len		+= str_len;	

	//6)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;	

	//------------------------------------------------------------------
	// add header line(SERVER)
	//------------------------------------------------------------------
	//1)add header name
	str_len	= strlen(&http_res_headername[RES_SERVER][0]);
	strcpy(http_con, &http_res_headername[RES_SERVER][0], str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add :
	*http_con		= 0x3a;		//:

	http_con		+= 1;
	total_len		+= 1;

	//3)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;	

	//4)add value
	str_len	= strlen(server_name);
	strcpy(http_con, server_name, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//5)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add header line(CONTENT LENGTH)
	//------------------------------------------------------------------
	//1)add header name
	str_len	= strlen(&http_res_headername[RES_CONTENT_LENGTH][0]);
	strcpy(http_con, &http_res_headername[RES_CONTENT_LENGTH][0], str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add :
	*http_con		= 0x3a;		//:

	http_con		+= 1;
	total_len		+= 1;

	//3)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;	

	//4)add value
	empty_buf(tmp_data, 10);
	str_len	= strlen(data);
	i2s(str_len, tmp_data);
	str_len	= strlen(tmp_data);
	strcpy(http_con, tmp_data, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//5)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add header line(CONTENT TYPE)
	//------------------------------------------------------------------
	//1)add header name
	str_len	= strlen(&http_res_headername[RES_CONTENT_TYPE][0]);
	strcpy(http_con, &http_res_headername[RES_CONTENT_TYPE][0], str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//2)add :
	*http_con		= 0x3a;		//:

	http_con		+= 1;
	total_len		+= 1;

	//3)add sp
	*http_con		= 0x20;		//space

	http_con		+= 1;
	total_len		+= 1;	

	//4)add value
	str_len	= strlen(content_type);
	strcpy(http_con, content_type, str_len);

	http_con		+= str_len;
	total_len		+= str_len;

	//5)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// NULL line
	//------------------------------------------------------------------
	//1)\r\n
	*http_con		= '\r';		// \r
	*(http_con+1)		= '\n';		// \n
	
	http_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add content
	//------------------------------------------------------------------
	//1)read data
	read_data();		//read data from file into NET_SEND_BUF(8mb)

	//2)add data
	str_len	= strlen((s8*)NET_SEND_BUF);
	strcpy(http_con, (s8*)NET_SEND_BUF, str_len);

	http_con		+= str_len;
	total_len		+= str_len;
	
	//-------------------------------------------------------------------------
	// http state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_HTTPRESPONSE;
	s32 old_state	= http_state;
	http_state	= *(p_hhttp_state_tab + http_state * HTTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_HTTP;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}
