//===========================================================================
// libn.c
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
#include	"prototype.h"
#include	"kernel.h"
#include	"keyboard.h"
#include	"tty.h"

//===========================================================================
// makechksum(u8 *pkg, u16 num)
//===========================================================================
PUBLIC 	u16 	makechksum(u8 *pkg, u16 num)
{
	u32	res = 0;
	u16 	*buf=(u16*)pkg;

	while(num > 1)
	{
		res += *buf++;
		num -= 2;
	}

	if(num > 0)
	{
		res += *(u8*)buf;
	}

	while(res>>16)
	{
		res = (res & 0xFFFF) + (res >> 16);
	}

	return (u16)~res;
}


//===========================================================================
// change string(192.168.131.1) to struct iaddr
//===========================================================================
PUBLIC void s2ip(struct iaddr * ip, s8 *buf)
{
	s8 tmpb[4];
	s32 len 	= strlen(buf);

	u8 *q = (u8*)ip;

	empty_buf(tmpb, 4);

	//chang . to \0
	s32 i,j;
	for(j=0, i=0; i<=len, q!=NULL; i++)
	{
		if(buf[i] == NULL)
		{
			break;
		}
		else if(buf[i] == 46)
		{			
			tmpb[j]= NULL;
			*q = s2i(tmpb);

			empty_buf(tmpb, 4);
			j = 0;
			q++;
		}
		else
		{
			tmpb[j] = buf[i];
			j++;
		}
	}

	tmpb[j]= NULL;
	*q = s2i(tmpb);
}

//===========================================================================
// change string(192.168.131.1) to struct iaddr
//===========================================================================
PUBLIC s8 chk_src_dest_ip(struct iaddr *src_ip, struct iaddr *dest_ip)
{
	if( (src_ip->addr1 == dest_ip->addr1) &&
	    (src_ip->addr2 == dest_ip->addr2) &&
	    (src_ip->addr3 == dest_ip->addr3) &&
	    (src_ip->addr4 == dest_ip->addr4) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//===========================================================================
// change string(192.168.131.1) to struct iaddr
//===========================================================================
PUBLIC s8 chk_src_dest_mac(struct hwaddr src_mac, struct hwaddr dest_mac)
{
	if( (src_mac.addr1 == dest_mac.addr1) &&
	    (src_mac.addr2 == dest_mac.addr2) &&
	    (src_mac.addr3 == dest_mac.addr3) &&
	    (src_mac.addr4 == dest_mac.addr4) &&
	    (src_mac.addr5 == dest_mac.addr5) &&
	    (src_mac.addr6 == dest_mac.addr6) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//===========================================================================
// print mac
//===========================================================================
PUBLIC s8 pr_mac(struct hwaddr mac)
{
	printk("mac:%x:%x:%x:%x:%x:%x.\n", mac.addr1, mac.addr2, mac.addr3, mac.addr4, mac.addr5, mac.addr6);
}
