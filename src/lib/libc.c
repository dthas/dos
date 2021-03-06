//===========================================================================
// lib.c
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
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC void i2s(int val, char *buf)
{
	int reminder 	= val % 10;
	val 		= val / 10;

	push(buf, reminder + '0');
	
	if(val != 0)
	{
		i2s(val, buf);
	}
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC void i2h(int val, char *buf)
{
	int tmp_v;
	int i = 28;	
	int j = 0;	

	s8 flg = 0;

	for(i=28; i>=0; i=i-4)
	{
		tmp_v = val >> i;
		tmp_v &= 0xF;
		
		if((tmp_v > 0) || (flg))
		{
			if(tmp_v < 9)
			{
				tmp_v = tmp_v + '0';
			}
			else
			{
				tmp_v = tmp_v + 87; 	//if tmp_v=10, then tmp_v = 87 + 10 = 97 = 'a'
			}
		
			*(buf+j) = tmp_v;
			j++;

			flg = 1;
		}
	}

	*(buf+j) = '\0';
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u8 	c2i(s8 ch)
{
	return(ch - 0x30);
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u8 	s2i(s8 * str)
{
	s32 len	= strlen(str);
	s32 sum = 0;

	s32 i,j,k;
	for(i=len-1, j=0; i>=0; i--,j++)
	{
		k 	= str[i] - '0';
		k 	= k * squ(j, 10);
		sum	= sum + k;
	}

	return sum;
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	s32 	squ(s32 num, s32 ind)
{
	s32 i; 
	s32 res = 1;
	for(i=0; i<num; i++)
	{
		res = res * ind;
	}

	return res;
}


//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u16 	big_little_16(u16 val)
{
	u16 t_val = val;
	
	return(((val<<8)|(t_val>>8)));
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u32 	big_little_32(u32 val)
{	
	return(((val>>24)&0xFF)|((val>>8)&0xFF00)|((val<<8)&0xFF0000)|((val<<24)&0xFF000000));
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u16 	little_big_16(u16 val)
{
	u16 t_val = val;
	
	return(((val<<8)|(t_val>>8)));
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC 	u32 	little_big_32(u32 val)
{	
	return(((val>>24)&0xFF)|((val>>8)&0xFF00)|((val<<8)&0xFF0000)|((val<<24)&0xFF000000));
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC void push(char *buf, char val)
{
	char tmp_buf[64];

	clear_buf(tmp_buf,64);

	strcopy(tmp_buf, buf);

	*buf = val;
	
	strcopy((buf+1), tmp_buf);
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC	void clear_buf(char *buf, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		*(buf+i) = '\0';
	}
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC	u8 num2bit(u16 num)
{
	u8 bit;
	switch(num)
	{
		case 0:
			bit = 0;
			break;
		case 0x1:
			bit = 1;
			break;
		case 0x3:
			bit = 2;
			break;
		case 0x7:
			bit = 3;
			break;
		case 0xF:
			bit = 4;
			break;
		case 0x1F:
			bit = 5;
			break;
		case 0x3F:
			bit = 6;
			break;
		case 0x7F:
			bit = 7;
			break;
		case 0xFF:
			bit = 8;
			break;
		case 0x1FF:
			bit = 9;
			break;
		case 0x3FF:
			bit = 10;
			break;
		case 0x7FF:
			bit = 11;
			break;
		case 0xFFF:
			bit = 12;
			break;
		case 0x1FFF:
			bit = 13;
			break;
		case 0x3FFF:
			bit = 14;
			break;
		case 0x7FFF:
		case 0xFFFE:
			bit = 15;
			break;
		case 0xFFFF:
			bit = 16;
			break;
	}

	return bit;
}

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC	u16 bit2num(u8 bit)
{
	u16 num;
	switch(bit)
	{
		case 0:
			num = 0;
			break;
		case 1:
			num = 0x1;
			break;
		case 2:
			num = 0x3;
			break;
		case 3:
			num = 0x7;
			break;
		case 4:
			num = 0xF;
			break;
		case 5:
			num = 0x1F;
			break;
		case 6:
			num = 0x3F;
			break;
		case 7:
			num = 0x7F;
			break;
		case 8:
			num = 0xFF;
			break;
		case 9:
			num = 0x1FF;
			break;
		case 10:
			num = 0x3FF;
			break;
		case 11:
			num = 0x7FF;
			break;
		case 12:
			num = 0xFFF;
			break;
		case 13:
			num = 0x1FFF;
			break;
		case 14:
			num = 0x3FFF;
			break;
		case 15:
			num = 0x7FFF;
			break;
		case 16:
			num = 0xFFFF;
			break;
	}

	return num;
}


//===========================================================================
// fill '\0' to an buffer
//===========================================================================
PUBLIC	void	empty_buf(u32 start_addr, u32 size_in_byte)
{
	u32 size= size_in_byte;
	s8 *p	= (s8*)start_addr;

	u32 i;
	for(i=0; i < size; i++)
	{
		p[i] = '\0';
	} 
}

//===========================================================================
// delay
//===========================================================================
PUBLIC	void	delay(u32 num)
{
	u32 i,j,k;

	for(i=0; i<num; i++)
	{
		for(j=0; j<100; j++)
		{
			for(k=0; k<100; k++)
			{
			}
		}
	}
}

//===========================================================================
// bcd_2_i(u8 num)
//===========================================================================
PUBLIC	u8	bcd_2_i(u8 num)
{
	u8 t;

	t = num >> 4;
	t = t * 10;

	num &= 0xF;
	t += num;

	return t;
}
