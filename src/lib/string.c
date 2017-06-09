//===========================================================================
// strcmp.c
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
#include	"global.h"
#include	"keyboard.h"
#include	"tty.h"

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC 	void strcopy(char *dest, char *src)
{
	while(*src != NULL)
	{
		*dest = *src;
		dest++;
		src++;
	}
}

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC 	void strcpy(char *dest, char *src, s32 len)
{
	s32 i;

	for(i=0; i<len; i++)
	{
		*dest++ = *src++;
	}	
}

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC 	void memcpy(u8 *dest, u8 *src, s32 len)
{
	s32 i;

	for(i=0; i<len; i++)
	{
		*dest++ = *src++;
	}	
}

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC 	int strlen(char *buf)
{
	int len = 0;

	while(*buf != NULL)
	{
		len++;
		buf++;
	}

	return len;
}

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC 	s8 strcmp(char *dest, char *src)
{
	s32	lend = strlen(dest);
	s32	lens = strlen(src);

	if(lend != lens)
	{
		return FALSE;
	}

	for(; lend > 0; lend--)
	{
		if(*dest++ != *src++)
		{
			break;
		}		
	}

	if(lend == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
