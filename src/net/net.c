//===========================================================================
// net.c
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
#include	"ipv4.h"
#include	"icmp.h"
#include	"net.h"
#include	"udp.h"
#include	"tcp.h"
#include	"prototype.h"


PUBLIC	u8	net_stack[STACK_SIZE];

//===========================================================================
// init tty
//===========================================================================
PUBLIC	void init_net()
{
	p_net_stack = net_stack;

//------for test
	s8 *str = "-init net-";
	disp_str(str);	
}

//===========================================================================
// net main
//===========================================================================
PUBLIC	void 	net_main()
{
	init_pci();

	init_rtl8029();

	init_ethernet();

	init_routetbl();

	init_tcp();	

	while(1)
	{
		struct s_package recv_pkg;
		struct s_package send_pkg;
		struct iaddr ip;
 
		if(rtl8029_receive_pkg(&recv_pkg))
		{
			pkg_process(&recv_pkg);			
		}
		
		struct s_mesg mesg;
		mesg.src_pid 	= PROC_NET;
		mesg.dest_pid 	= PROC_NET;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case NET_SEND:
					rtl8029_send_pkg(mesg.msg.buf1);
					break;
				case ICMP_0_8:
					s2ip(&ip, mesg.msg.buf1);
					icmp_process(&send_pkg, ip, mesg.msg.parm1, mesg.msg.parm2, 0, 0);
					break;
				case ICMP_13_14:
					icmp_process(&send_pkg, get_default_gw(), mesg.msg.parm1, mesg.msg.parm2, 0, 0);
					break;
				case NETSTAT:
					pr_routetbl();
					break;
				case IFCONFIG:
					pr_ethernet();
					break;
				case TRACEROUTE:
					s2ip(&ip, mesg.msg.buf1);
					sys_traceroute(&send_pkg, ip);
					break;
				case DY_RIP1:
					dyroutes_process(&send_pkg, DY_RIP1, mesg.msg.parm1);
					break;
				case DY_RIP2:
					dyroutes_process(&send_pkg, DY_RIP2, mesg.msg.parm1);
					break;
				case DY_OSPF2:
					dyroutes_process(&send_pkg, DY_OSPF2, mesg.msg.parm1);
					break;
				case DY_BGP:
					dyroutes_process(&send_pkg, DY_BGP, mesg.msg.parm1);
					break;
				case IGMP3:
					igmp_process(&send_pkg, IGMP3, mesg.msg.parm1);
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
// PUBLIC	void 	pkg_process(struct s_package *pkg)
//===========================================================================
PUBLIC	void 	pkg_process(struct s_package *pkg)
{
	struct s_mesg mesg;

	u32 hour, min, sec, temp;

	struct s_package tpkg;
	u16 op, port;

	struct frame8023_header *fh 	= (struct frame8023_header *)(pkg->buffer);
	struct frame_arp_req *fc 	= (struct frame_arp_req *)(&(pkg->buffer[FRAME_HEADER_LENGTH]));

	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH]));

	struct s_icmp_header *icmph 	= (struct s_icmp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	struct s_icmp_timestamp *icmpt	= (struct s_icmp_timestamp *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + ICMP_HEADER_LENGTH_0_8]));

	struct s_udp_header *udph 	= (struct s_udp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	switch(big_little_16(fh->type))
	{
		case FRAME_IP_TYPE:

			if(!chk_src_dest_ip(pnet_dev->ip,iph->dst_ip))
			{				
				ip_forward_v4(pkg);
				return;
			}			
			
			//drop the broadcast package that sent by myself
			if(chk_src_dest_mac(pnet_dev->mac,fh->src_mac))
			{
				return;
			}
			
			switch(iph->protocol)
			{
				case PROTOCOL_ICMP:					
					switch(icmph->type)
					{
						case 0:
							printk("%d.%d.%d.%d is alive\n",iph->src_ip.addr1, iph->src_ip.addr2, iph->src_ip.addr3, iph->src_ip.addr4);
							sys_reset_cmdline();
							break;
						case 8:
							icmp_process(pkg, iph->src_ip, 0, icmph->code, icmph->iden, icmph->seq);
							break;
						case 13:
							icmp_process(pkg, iph->src_ip, 14, icmph->code, icmph->iden, icmph->seq);
							break;
						case 14:
							temp	= big_little_32(icmpt->recv_timestamp);
							sec	= temp / 1000 % 60;
							min	= temp / (1000 * 60) % 60;
							hour	= temp / (1000 * 3600);
							hour	= hour + UTC_BEIJING;							
							
							printk("net time is %d:%d:%d\n", hour, min, sec);
							sys_reset_cmdline();
							break;
						default:
							break;
					}
					break;
					
				case PROTOCOL_UDP:
					
					if(big_little_16(udph->dst_port) > 1024)
					{
						port	= big_little_16(udph->src_port);
					}
					else
					{
						port	= big_little_16(udph->dst_port);
					}

					switch(port)
					{
						case PORT_DHCP_CLIENT:
						case PORT_DHCP_SERVER:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_DHCP;
							mesg.type	= DHCP_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;
						case PORT_DNS:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_DNS;
							mesg.type	= DNS_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;
						default:
							//case PORT_TFTP:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_TFTP;
							mesg.type	= TFTP_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;
					}

					break;
				
				case PROTOCOL_TCP:
					if(big_little_16(tcph->dst_port) > 1024)
					{
						port	= big_little_16(tcph->src_port);
					}
					else
					{
						port	= big_little_16(tcph->dst_port);
					}					

					switch(port)
					{
						case PORT_FTP_DATA:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_FTP;
							mesg.type	= FTP_DATA;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;

						case PORT_FTP_CTRL:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_FTP;
							mesg.type	= FTP_CTRL;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;

						case PORT_HTTP:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_HTTP;
							mesg.type	= HTTP_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;

						case PORT_SMTP:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_SMTP;
							mesg.type	= SMTP_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;

						case PORT_POP3:
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_POP3;
							mesg.type	= POP3_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;

						case PORT_TELNET:							
							mesg.msg.buf1	= pkg;
							mesg.dest_pid	= PROC_TELNET;
							mesg.type	= TELNET_PROCESS;
							mesg.func	= SYN;
							send_msg(&mesg);
							break;
						default:
							break;
					}
				
					break;

				default:
					break;
			}
 
			break;
		case FRAME_ARP_TYPE:			
			//check if the package is send to me
			if(!chk_src_dest_ip(pnet_dev->ip,fc->dst_ip))
			{
				return;
			}		
			
			op = big_little_16(fc->op);

			switch(op)
			{
				case F_OP_ARP_REQUEST:
					op = F_OP_ARP_REPLY;
					build_frame(&tpkg, FRAME_ARP_TYPE, op, fc->src_mac, fc->src_ip, pnet_dev->mac, fc->dst_ip);
					rtl8029_send_pkg(&tpkg);
					break;
				case F_OP_ARP_REPLY:
					update_routetable(fc->src_ip, fc->src_mac);
					break;
				default:
					break;
			}			
			break;
		case FRAME_RARP_TYPE:
			break;
		default:
			break;
	}
}
