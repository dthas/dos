//===========================================================================
// keyboard.c
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
#include	"global.h"
#include	"prototype.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"tty.h"
#include	"keyboard.h"

extern	current_tty;


PRIVATE	struct keyb_buf	kb_buf;

PRIVATE u32 tcode;
PRIVATE s8 flg_e0;
PRIVATE s8 flg_e1;
PRIVATE s8 flg_shift;
PRIVATE s8 flg_ctrl;
PRIVATE s8 flg_alt;
PRIVATE s8 flg_cap = FALSE;
PRIVATE s8 flg_numlock = TRUE;
//===========================================================================
// keyboard handler
//===========================================================================
PUBLIC	void keyb_handler()
{
	u8	scode = in_8(KB_PORT);

	if (kb_buf.count > (KB_INPUT_SIZE - 1))
	{
		kb_buf.write_ptr	= kb_buf.buffer;
		kb_buf.count 		= 0;
	}
	else
	{
		*(kb_buf.write_ptr) 	= scode;
		kb_buf.write_ptr++;
		kb_buf.count++;
	}

	proc_tab[PROC_TTY].status 	= PROC_READY;
}

//===========================================================================
// init keyboard 
//===========================================================================
PUBLIC	void init_keyb()
{
	kb_buf.write_ptr 	= kb_buf.buffer;
	kb_buf.read_ptr 	= kb_buf.buffer;
	kb_buf.cur_tty		= current_tty;
	kb_buf.count		= 0;

	en_irq(KEYBOARD_IRQ);

//------for test
	char *str = "+en keyb+";
	disp_str(str);	
//------for test
}


//===========================================================================
// keyboard read
//===========================================================================
PUBLIC	void keyb_read()
{
	
	while (kb_buf.count > 0)
	{			
		u8 scode 	= *(kb_buf.read_ptr);
			
		char_exec(scode);
		kb_buf.read_ptr++;
		kb_buf.count--;	
	}		

	kb_buf.write_ptr 	= kb_buf.buffer;
	kb_buf.read_ptr 	= kb_buf.buffer;	
} 

//===========================================================================
// analyze each character
//===========================================================================
PUBLIC	void char_exec(u8 scode)
{
	if (scode == 0xe0)
	{
		flg_e0 = TRUE;
		tcode  = scode;

		return 1;
	}

	if(flg_e0 == TRUE)
	{
		tcode <<= 8;
		tcode |= scode;
	}
	else
	{
		tcode = scode;
	}

	switch (tcode)
	{
//-------------------------------------------------------------------------
// a ~ z, A ~ Z
//-------------------------------------------------------------------------
		case A_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_A : ASC_LCASE_A);
			break;
		case A_BREAK_CODE:
			break;
		case B_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_B : ASC_LCASE_B);
			break;
		case B_BREAK_CODE:
			break;
		case C_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_C : ASC_LCASE_C);
			break;
		case C_BREAK_CODE:
			break;
		case D_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_D : ASC_LCASE_D);
			break;
		case D_BREAK_CODE:
			break;
		case E_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_E : ASC_LCASE_E);
			break;
		case E_BREAK_CODE:
			break;
		case F_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_F : ASC_LCASE_F);
			break;
		case F_BREAK_CODE:
			break;
		case G_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_G : ASC_LCASE_G);
			break;
		case G_BREAK_CODE:
			break;
		case H_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_H : ASC_LCASE_H);
			break;
		case H_BREAK_CODE:
			break;
		case I_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_I : ASC_LCASE_I);
			break;
		case I_BREAK_CODE:
			break;
		case J_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_J : ASC_LCASE_J);
			break;
		case J_BREAK_CODE:
			break;
		case K_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_K : ASC_LCASE_K);
			break;
		case K_BREAK_CODE:
			break;
		case L_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_L : ASC_LCASE_L);
			break;
		case L_BREAK_CODE:
			break;
		case M_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_M : ASC_LCASE_M);
			break;
		case M_BREAK_CODE:
			break;
		case N_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_N : ASC_LCASE_N);
			break;
		case N_BREAK_CODE:
			break;
		case O_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_O : ASC_LCASE_O);
			break;
		case O_BREAK_CODE:
			break;
		case P_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_P : ASC_LCASE_P);
			break;
		case P_BREAK_CODE:
			break;
		case Q_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_Q : ASC_LCASE_Q);
			break;
		case Q_BREAK_CODE:
			break;
		case R_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_R : ASC_LCASE_R);
			break;
		case R_BREAK_CODE:
			break;
		case S_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_S : ASC_LCASE_S);
			break;
		case S_BREAK_CODE:
			break;
		case T_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_T : ASC_LCASE_T);
			break;
		case T_BREAK_CODE:
			break;
		case U_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_U : ASC_LCASE_U);
			break;
		case U_BREAK_CODE:
			break;
		case V_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_V : ASC_LCASE_V);
			break;
		case V_BREAK_CODE:
			break;
		case W_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_W : ASC_LCASE_W);
			break;
		case W_BREAK_CODE:
			break;
		case X_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_X : ASC_LCASE_X);
			break;
		case X_BREAK_CODE:
			break;
		case Y_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_Y : ASC_LCASE_Y);
			break;
		case Y_BREAK_CODE:
			break;
		case Z_MAKE_CODE:
			tty_buf_write(flg_shift != flg_cap ? ASC_Z : ASC_LCASE_Z);
			break;
		case Z_BREAK_CODE:
			break;
//-------------------------------------------------------------------------
// 0 ~ 9(main keyboard)
//-------------------------------------------------------------------------
		case NUM_1_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_1 : ASC_EXCLA);
			break;
		case NUM_1_BREAK_CODE:
			break;
		case NUM_2_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_2 : ASC_AT);
			break;
		case NUM_2_BREAK_CODE:
			break;
		case NUM_3_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_3 : ASC_POUND);
			break;
		case NUM_3_BREAK_CODE:
			break;
		case NUM_4_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_4 : ASC_DOLLAR);
			break;
		case NUM_4_BREAK_CODE:
			break;
		case NUM_5_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_5 : ASC_PERCENT);
			break;
		case NUM_5_BREAK_CODE:
			break;
		case NUM_6_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_6 : ASC_TSQUARE);
			break;
		case NUM_6_BREAK_CODE:
			break;
		case NUM_7_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_7 : ASC_AND);
			break;
		case NUM_7_BREAK_CODE:
			break;
		case NUM_8_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_8 : ASC_MUL);
			break;
		case NUM_8_BREAK_CODE:
			break;
		case NUM_9_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_9 : ASC_L_RBLACKET);
			break;
		case NUM_9_BREAK_CODE:
			break;
		case NUM_0_MAKE_CODE:
			tty_buf_write(flg_shift == FALSE ? ASC_0 : ASC_R_RBLACKET);
			break;
		case NUM_0_BREAK_CODE:
			break;
//-------------------------------------------------------------------------
// 0 ~ 9(key pad)
//-------------------------------------------------------------------------
		case KP_1_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_1);
			}
			break;
		case KP_1_BREAK_CODE:
			break;
		case KP_2_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					direction_exec(DOWN);
				}
				else
				{
					tty_buf_write(ASC_2);
				}
			}
			break;
		case KP_2_BREAK_CODE:
			break;
		case KP_3_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					scroll_down();
				}
				else
				{
					tty_buf_write(ASC_3);
				}
			}
			break;
		case KP_3_BREAK_CODE:
			break;
		case KP_4_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					direction_exec(LEFT);
				}
				else
				{
					tty_buf_write(ASC_4);
				}
			}
			break;
		case KP_4_BREAK_CODE:
			break;
		case KP_5_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_5);
			}
			break;
		case KP_5_BREAK_CODE:
			break;
		case KP_6_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					direction_exec(RIGHT);
				}
				else
				{
					tty_buf_write(ASC_6);
				}
			}
			break;
		case KP_6_BREAK_CODE:
			break;
		case KP_7_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_7);
			}
			break;
		case KP_7_BREAK_CODE:
			break;
		case KP_8_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					direction_exec(UP);
				}
				else
				{
					tty_buf_write(ASC_8);
				}
			}
			break;
		case KP_8_BREAK_CODE:
			break;
		case KP_9_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				if(flg_shift == TRUE)
				{
					scroll_up();
				}
				else
				{
					tty_buf_write(ASC_9);
				}
			}
			break;
		case KP_9_BREAK_CODE:
			break;
		case KP_0_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_0);
			}
			break;
		case KP_0_BREAK_CODE:
			break;
		
//-------------------------------------------------------------------------
// symbols(main keyboard)
//-------------------------------------------------------------------------		
		case E_COMMA_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_NOT : ASC_E_COMMA);
			break;
		case E_COMMA_BREAK_CODE:
			break;
		case HYPHEN_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_UNDERLINE : ASC_MINUS);
			break;
		case HYPHEN_BREAK_CODE:
			break;
		case EQUAL_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_PLUS : ASC_EQUAL);
			break;
		case EQUAL_BREAK_CODE:
			break;
		case BACKSLASH_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_V_LINE : ASC_E_SLASH);
			break;
		case BACKSLASH_BREAK_CODE:
			break;

		case L_BLACKET_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_L_BLACKET : ASC_L_SBRACKET);
			break;
		case L_BLACKET_BREAK_CODE:
			break;
		case R_BLACKET_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_R_BLACKET : ASC_R_SBRACKET);
			break;
		case R_BLACKET_BREAK_CODE:
			break;
		case SEMICOLON_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_COLON : ASC_SEMICOLON);
			break;
		case SEMICOLON_BREAK_CODE:
			break;
		case SQMARK_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_DBL_QUOTA : ASC_SINGLE_QUOTA);
			break;
		case SQMARK_BREAK_CODE:
			break;
		case COMMA_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_BLOW : ASC_COMMA);
			break;
		case COMMA_BREAK_CODE:
			break;
		case FULLSTOP_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_ABOVE : ASC_DOT);
			break;
		case FULLSTOP_BREAK_CODE:
			break;
		case SLASH_MAKE_CODE:
			tty_buf_write(flg_shift == TRUE ? ASC_QUESTION : ASC_DIV);
			break;
		case SLASH_BREAK_CODE:
			break;

		case SPACE_MAKE_CODE:
			tty_buf_write(ASC_SPACE);
			break;
		case SPACE_BREAK_CODE:
			break;
//-------------------------------------------------------------------------
// symbols(key pad)
//-------------------------------------------------------------------------
		case KP_DIV_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_DIV);
			}
			flg_e0 = FALSE;
			break;
		case KP_DIV_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case KP_MUL_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_MUL);
			}
			break;
		case KP_MUL_BREAK_CODE:
			break;
		case KP_SUB_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_MINUS);
			}
			break;
		case KP_SUB_BREAK_CODE:
			break;
		case KP_ADD_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				tty_buf_write(ASC_PLUS);
			}
			break;
		case KP_ADD_BREAK_CODE:
			break;
		case KP_DOT_MAKE_CODE:
			if(flg_numlock == TRUE)
			{			
				if(flg_shift == TRUE)
				{
					delete_exec();
				}
				else
				{
					tty_buf_write(ASC_DOT);
				}
			}
			break;
		case KP_DOT_BREAK_CODE:
			break;
		case KP_ENTER_MAKE_CODE:
			if(flg_numlock == TRUE)
			{			
				enter_exec();
			}
			flg_e0 = FALSE;
			break;
		case KP_ENTER_BREAK_CODE:
			flg_e0 = FALSE;
			break;
//-------------------------------------------------------------------------
// function(main keyboard). 
//-------------------------------------------------------------------------
		case ESC_MAKE_CODE:
			if(flg_ctrl == TRUE)
			{
				change_tty(TTY0);
			}	
			break;			
		case ESC_BREAK_CODE:
			break;
		case F1_MAKE_CODE:
			if(flg_ctrl == TRUE)
			{
				change_tty(TTY1);
			}	
			break;		
		case F1_BREAK_CODE:
			break;
		case F2_MAKE_CODE:
			if(flg_ctrl == TRUE)
			{
				change_tty(TTY2);
			}	
			break;			
		case F2_BREAK_CODE:
			break;
		case F3_MAKE_CODE:
			if(flg_ctrl == TRUE)
			{
				change_tty(TTY3);
			}	
			break;			
		case F3_BREAK_CODE:
			break;
		case F4_MAKE_CODE:			
		case F4_BREAK_CODE:
			break;
		case F5_MAKE_CODE:			
		case F5_BREAK_CODE:
			break;
		case F6_MAKE_CODE:			
		case F6_BREAK_CODE:
			break;
		case F7_MAKE_CODE:			
		case F7_BREAK_CODE:
			break;
		case F8_MAKE_CODE:			
		case F8_BREAK_CODE:
			break;
		case F9_MAKE_CODE:			
		case F9_BREAK_CODE:
			break;
		case F10_MAKE_CODE:			
		case F10_BREAK_CODE:
			break;
		case F11_MAKE_CODE:			
		case F11_BREAK_CODE:
			break;
		case F12_MAKE_CODE:			
		case F12_BREAK_CODE:
			break;

		case LSHIFT_MAKE_CODE:	
			flg_shift = TRUE;
			break;		
		case LSHIFT_BREAK_CODE:
			flg_shift = FALSE;
			break;
		case RSHIFT_MAKE_CODE:	
			flg_shift = TRUE;
			break;		
		case RSHIFT_BREAK_CODE:
			flg_shift = FALSE;
			break;
		case LCTRL_MAKE_CODE:	
			flg_ctrl = TRUE;
			break;		
		case LCTRL_BREAK_CODE:
			flg_ctrl = FALSE;
			break;
		case RCTRL_MAKE_CODE:	
			flg_e0 = FALSE;
			flg_ctrl = TRUE;
			break;		
		case RCTRL_BREAK_CODE:
			flg_e0 = FALSE;
			flg_ctrl = FALSE;
			break;
		case LALT_MAKE_CODE:	
			flg_alt = TRUE;
			break;		
		case LALT_BREAK_CODE:
			flg_alt = FALSE;
			break;
		case RALT_MAKE_CODE:
			flg_e0 = FALSE;	
			flg_alt = TRUE;
			break;		
		case RALT_BREAK_CODE:
			flg_e0 = FALSE;
			flg_alt = FALSE;
			break;
		case CAPS_MAKE_CODE:
			if(flg_cap == TRUE)
			{
				flg_cap = FALSE;
			}	
			else
			{
				flg_cap = TRUE;
			}
			
			break;		
		case CAPS_BREAK_CODE:
			break;
//-------------------------------------------------------------------------
// function(mid keyboard)
//-------------------------------------------------------------------------
		case PAGEUP_MAKE_CODE:
			scroll_up();
			flg_e0 = FALSE;
			break;
		case PAGEUP_BREAK_CODE:
			flg_e0 = FALSE;
			break;

		case PAGEDOWN_MAKE_CODE:
			scroll_down();
			flg_e0 = FALSE;
			break;
		case PAGEDOWN_BREAK_CODE:
			flg_e0 = FALSE;
			break;

		case DELETE_MAKE_CODE:	
			delete_exec();
			flg_e0 = FALSE;
			break;		
		case DELETE_BREAK_CODE:
			flg_e0 = FALSE;
			break;

		case ARROWUP_MAKE_CODE:	
			direction_exec(UP);
			flg_e0 = FALSE;
			break;		
		case ARROWUP_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case ARROWLEFT_MAKE_CODE:	
			direction_exec(LEFT);
			flg_e0 = FALSE;
			break;		
		case ARROWLEFT_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case ARROWDOWN_MAKE_CODE:	
			direction_exec(DOWN);
			flg_e0 = FALSE;
			break;		
		case ARROWDOWN_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case ARROWRIGHT_MAKE_CODE:	
			direction_exec(RIGHT);
			flg_e0 = FALSE;
			break;		
		case ARROWRIGHT_BREAK_CODE:
			flg_e0 = FALSE;
			break;

		case HOME_MAKE_CODE:	
			home_exec();
			flg_e0 = FALSE;
			break;		
		case HOME_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case END_MAKE_CODE:	
			end_exec();
			flg_e0 = FALSE;
			break;		
		case END_BREAK_CODE:
			flg_e0 = FALSE;
			break;

//-------------------------------------------------------------------------
// function(key pad)
//-------------------------------------------------------------------------
		case KP_NUM_MAKE_CODE:
			if(flg_numlock == TRUE)
			{
				flg_numlock = FALSE;
			}	
			else
			{
				flg_numlock = TRUE;
			}
			
			break;		
		case KP_NUM_BREAK_CODE:
			break;

//-------------------------------------------------------------------------
// editing function(main keyboard)
//-------------------------------------------------------------------------
		case TAB_MAKE_CODE:	
			tab_exec();
			break;		
		case TAB_BREAK_CODE:
			break;
		case BACKSPACE_MAKE_CODE:	
			backspace_exec();
			break;		
		case BACKSPACE_BREAK_CODE:
			break;
		case ENTER_MAKE_CODE:	
			enter_exec();
			break;		
		case ENTER_BREAK_CODE:
			break;
//-------------------------------------------------------------------------
// ignore keys
//-------------------------------------------------------------------------
		case LWIN_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case LWIN_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case RWIN_MAKE_CODE:	
			flg_e0 = FALSE;
			shutdown();
			break;		
		case RWIN_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case APPS_MAKE_CODE:
			flg_e0 = FALSE;
			reboot();
			break;		
		case APPS_BREAK_CODE:
			flg_e0 = FALSE;
			break;


		case SLEEP_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case SLEEP_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case WAKEUP_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case WAKEUP_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case POWER_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case POWER_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case PRINTSCREEN_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case PRINTSCREEN_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case SCROLL_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case SCROLL_BREAK_CODE:
			flg_e0 = FALSE;
			break;
		case INSERT_MAKE_CODE:	
			flg_e0 = FALSE;
			break;		
		case INSERT_BREAK_CODE:
			flg_e0 = FALSE;
			break;

		case PAUSE_MAKE_CODE:	
			flg_e1 = FALSE;
			break;	

//-------------------------------------------------------------------------
// default
//-------------------------------------------------------------------------
		default:			
			break;
	}
} 
