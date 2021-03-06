//===========================================================================
// printf.c
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
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC	s32 printf(char *cmd, ...)
{
	char *value = (char*)((char*)(&cmd) + 4);

	s32 ret = printf_exec(cmd, value);

	return ret;
}

//===========================================================================
// PUBLIC	s32 printf(char *cmd, ...)
//===========================================================================
PUBLIC	s32 printf_exec(char *cmd, char *value)
{
	char output_buf[512];
	char tmp_buf[64];
	s32  len = 0;
	char *head = output_buf;
	int  num;
	int  ret;
	int  i=0;
	
	while(*cmd)
	{
		if (*cmd == '%')
		{
			switch(*(++cmd))
			{
				case 'd' :
					clear_buf(tmp_buf,64);

					num = *(int*)(value);
					i2s(num, tmp_buf);
											
					len 	+= strlen(tmp_buf);			
					strcopy(head, tmp_buf);
					
					head 	+= len;
					value 	+= 4;
					cmd++;
					break;
				case 's' :					
					len 	+= strlen((char*)(*(int*)value));  			
					strcopy(head, (char*)(*(int*)value));
					
					head 	+= len;
					value 	+= 4;
					cmd++;
					break;
				case 'c' :
					*head = *(value);
										
					head++;
					value 	+= 4;
					cmd++;
					break;
				case 'x' :
					clear_buf(tmp_buf,64);	

					num = *(int*)(value);
					i2h(num, tmp_buf);
					len 	+= strlen(tmp_buf);			
					strcopy(head, tmp_buf);	

					head 	+= len;
					value 	+= 4;
					cmd++;
					break;
				default:					
					break;
			}
				
		}
		else
		{
			*head = *cmd;
			head++;
			cmd++;
		}
	}

	puts(output_buf, (head - output_buf));
}
