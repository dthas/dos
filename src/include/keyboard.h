//===========================================================================
// keyboard.h
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
#ifndef	_X_KEYBOARD_H_
#define	_X_KEYBOARD_H_

#define	BREAK_SET_CODE	0x80
#define	KB_INPUT_SIZE	64
#define KB_PORT		0x60

//-------------------------------------------------------------------------
// main keyboard
//-------------------------------------------------------------------------

// 1th line
#define	ESC_MAKE_CODE		0x1
#define	ESC_BREAK_CODE		(0x1 + BREAK_SET_CODE)

#define	F1_MAKE_CODE		0x3B
#define	F1_BREAK_CODE		(0x3B + BREAK_SET_CODE)

#define	F2_MAKE_CODE		0x3C
#define	F2_BREAK_CODE		(0x3C + BREAK_SET_CODE)

#define	F3_MAKE_CODE		0x3D
#define	F3_BREAK_CODE		(0x3D + BREAK_SET_CODE)

#define	F4_MAKE_CODE		0x3E
#define	F4_BREAK_CODE		(0x3E + BREAK_SET_CODE)

#define	F5_MAKE_CODE		0x3F
#define	F5_BREAK_CODE		(0x3F + BREAK_SET_CODE)

#define	F6_MAKE_CODE		0x40
#define	F6_BREAK_CODE		(0x40 + BREAK_SET_CODE)

#define	F7_MAKE_CODE		0x41
#define	F7_BREAK_CODE		(0x41 + BREAK_SET_CODE)

#define	F8_MAKE_CODE		0x42
#define	F8_BREAK_CODE		(0x42 + BREAK_SET_CODE)

#define	F9_MAKE_CODE		0x43
#define	F9_BREAK_CODE		(0x43 + BREAK_SET_CODE)

#define	F10_MAKE_CODE		0x44
#define	F10_BREAK_CODE		(0x44 + BREAK_SET_CODE)

#define	F11_MAKE_CODE		0x57
#define	F11_BREAK_CODE		(0x57 + BREAK_SET_CODE)

#define	F12_MAKE_CODE		0x58
#define	F12_BREAK_CODE		(0x58 + BREAK_SET_CODE)

// 2th line
#define	E_COMMA_MAKE_CODE	0x29
#define	E_COMMA_BREAK_CODE	(0x29 + BREAK_SET_CODE)

#define	NUM_1_MAKE_CODE		0x2
#define	NUM_1_BREAK_CODE	(0x2 + BREAK_SET_CODE)

#define	NUM_2_MAKE_CODE		0x3
#define	NUM_2_BREAK_CODE	(0x3 + BREAK_SET_CODE)

#define	NUM_3_MAKE_CODE		0x4
#define	NUM_3_BREAK_CODE	(0x4 + BREAK_SET_CODE)

#define	NUM_4_MAKE_CODE		0x5
#define	NUM_4_BREAK_CODE	(0x5 + BREAK_SET_CODE)

#define	NUM_5_MAKE_CODE		0x6
#define	NUM_5_BREAK_CODE	(0x6 + BREAK_SET_CODE)

#define	NUM_6_MAKE_CODE		0x7
#define	NUM_6_BREAK_CODE	(0x7 + BREAK_SET_CODE)

#define	NUM_7_MAKE_CODE		0x8
#define	NUM_7_BREAK_CODE	(0x8 + BREAK_SET_CODE)

#define	NUM_8_MAKE_CODE		0x9
#define	NUM_8_BREAK_CODE		(0x9 + BREAK_SET_CODE)

#define	NUM_9_MAKE_CODE		0xA
#define	NUM_9_BREAK_CODE	(0xA + BREAK_SET_CODE)

#define	NUM_0_MAKE_CODE		0xB
#define	NUM_0_BREAK_CODE	(0xB + BREAK_SET_CODE)

#define	HYPHEN_MAKE_CODE	0xC
#define	HYPHEN_BREAK_CODE	(0xC + BREAK_SET_CODE)

#define	EQUAL_MAKE_CODE		0xD
#define	EQUAL_BREAK_CODE	(0xD + BREAK_SET_CODE)

#define	BACKSLASH_MAKE_CODE	0x2B
#define	BACKSLASH_BREAK_CODE	(0x2B + BREAK_SET_CODE)

#define	BACKSPACE_MAKE_CODE	0xE
#define	BACKSPACE_BREAK_CODE	(0xE + BREAK_SET_CODE)

// 3th line
#define	TAB_MAKE_CODE		0xF
#define	TAB_BREAK_CODE		(0xF + BREAK_SET_CODE)

#define	Q_MAKE_CODE		0x10
#define	Q_BREAK_CODE		(0x10 + BREAK_SET_CODE)

#define	W_MAKE_CODE		0x11
#define	W_BREAK_CODE		(0x11 + BREAK_SET_CODE)

#define	E_MAKE_CODE		0x12
#define	E_BREAK_CODE		(0x12 + BREAK_SET_CODE)

#define	R_MAKE_CODE		0x13
#define	R_BREAK_CODE		(0x13 + BREAK_SET_CODE)

#define	T_MAKE_CODE		0x14
#define	T_BREAK_CODE		(0x14 + BREAK_SET_CODE)

#define	Y_MAKE_CODE		0x15
#define	Y_BREAK_CODE		(0x15 + BREAK_SET_CODE)

#define	U_MAKE_CODE		0x16
#define	U_BREAK_CODE		(0x16 + BREAK_SET_CODE)

#define	I_MAKE_CODE		0x17
#define	I_BREAK_CODE		(0x17 + BREAK_SET_CODE)

#define	O_MAKE_CODE		0x18
#define	O_BREAK_CODE		(0x18 + BREAK_SET_CODE)

#define	P_MAKE_CODE		0x19
#define	P_BREAK_CODE		(0x19 + BREAK_SET_CODE)

#define	L_BLACKET_MAKE_CODE	0x1A
#define	L_BLACKET_BREAK_CODE	(0x1A + BREAK_SET_CODE)

#define	R_BLACKET_MAKE_CODE	0x1B
#define	R_BLACKET_BREAK_CODE	(0x1B + BREAK_SET_CODE)

#define	ENTER_MAKE_CODE		0x1C
#define	ENTER_BREAK_CODE	(0x1C + BREAK_SET_CODE)


// 4th line
#define	CAPS_MAKE_CODE		0x3A
#define	CAPS_BREAK_CODE		(0x3A + BREAK_SET_CODE)

#define	A_MAKE_CODE		0x1E
#define	A_BREAK_CODE		(0x1E + BREAK_SET_CODE)

#define	S_MAKE_CODE		0x1F
#define	S_BREAK_CODE		(0x1F + BREAK_SET_CODE)

#define	D_MAKE_CODE		0x20
#define	D_BREAK_CODE		(0x20 + BREAK_SET_CODE)

#define	F_MAKE_CODE		0x21
#define	F_BREAK_CODE		(0x21 + BREAK_SET_CODE)

#define	G_MAKE_CODE		0x22
#define	G_BREAK_CODE		(0x22 + BREAK_SET_CODE)

#define	H_MAKE_CODE		0x23
#define	H_BREAK_CODE		(0x23 + BREAK_SET_CODE)

#define	J_MAKE_CODE		0x24
#define	J_BREAK_CODE		(0x24 + BREAK_SET_CODE)

#define	K_MAKE_CODE		0x25
#define	K_BREAK_CODE		(0x25 + BREAK_SET_CODE)

#define	L_MAKE_CODE		0x26
#define	L_BREAK_CODE		(0x26 + BREAK_SET_CODE)

#define	SEMICOLON_MAKE_CODE	0x27
#define	SEMICOLON_BREAK_CODE	(0x27 + BREAK_SET_CODE)

#define	SQMARK_MAKE_CODE	0x28
#define	SQMARK_BREAK_CODE	(0x28 + BREAK_SET_CODE)


// 5th line
#define	LSHIFT_MAKE_CODE	0x2A
#define	LSHIFT_BREAK_CODE	(0x2A + BREAK_SET_CODE)

#define	Z_MAKE_CODE		0x2C
#define	Z_BREAK_CODE		(0x2C + BREAK_SET_CODE)

#define	X_MAKE_CODE		0x2D
#define	X_BREAK_CODE		(0x2D + BREAK_SET_CODE)

#define	C_MAKE_CODE		0x2E
#define	C_BREAK_CODE		(0x2E + BREAK_SET_CODE)

#define	V_MAKE_CODE		0x2F
#define	V_BREAK_CODE		(0x2F + BREAK_SET_CODE)

#define	B_MAKE_CODE		0x30
#define	B_BREAK_CODE		(0x30 + BREAK_SET_CODE)

#define	N_MAKE_CODE		0x31
#define	N_BREAK_CODE		(0x31 + BREAK_SET_CODE)

#define	M_MAKE_CODE		0x32
#define	M_BREAK_CODE		(0x32 + BREAK_SET_CODE)

#define	COMMA_MAKE_CODE		0x33
#define	COMMA_BREAK_CODE	(0x33 + BREAK_SET_CODE)

#define	FULLSTOP_MAKE_CODE	0x34
#define	FULLSTOP_BREAK_CODE	(0x34 + BREAK_SET_CODE)

#define	SLASH_MAKE_CODE		0x35
#define	SLASH_BREAK_CODE	(0x35 + BREAK_SET_CODE)

#define	RSHIFT_MAKE_CODE	0x36
#define	RSHIFT_BREAK_CODE	(0x36 + BREAK_SET_CODE)


// 6th line
#define	LCTRL_MAKE_CODE		0x1D
#define	LCTRL_BREAK_CODE	(0x1D + BREAK_SET_CODE)

#define	LWIN_MAKE_CODE		0xE05B
#define	LWIN_BREAK_CODE		(0xE05B + BREAK_SET_CODE)

#define	LALT_MAKE_CODE		0x38
#define	LALT_BREAK_CODE		(0x38 + BREAK_SET_CODE)

#define	SPACE_MAKE_CODE		0x39
#define	SPACE_BREAK_CODE	(0x39 + BREAK_SET_CODE)

#define	RALT_MAKE_CODE		0xE038
#define	RALT_BREAK_CODE		(0xE038 + BREAK_SET_CODE)

#define	RWIN_MAKE_CODE		0xE05C
#define	RWIN_BREAK_CODE		(0xE05C + BREAK_SET_CODE)

#define	APPS_MAKE_CODE		0xE05D
#define	APPS_BREAK_CODE		(0xE05D + BREAK_SET_CODE)

#define	RCTRL_MAKE_CODE		0xE01D
#define	RCTRL_BREAK_CODE	(0xE01D + BREAK_SET_CODE)



//-------------------------------------------------------------------------
// mid keyboard
//-------------------------------------------------------------------------
// 1th line
#define	SLEEP_MAKE_CODE		0xE05F
#define	SLEEP_BREAK_CODE	(0xE05F + BREAK_SET_CODE)

#define	WAKEUP_MAKE_CODE	0xE063
#define	WAKEUP_BREAK_CODE	(0xE063 + BREAK_SET_CODE)

#define	POWER_MAKE_CODE		0xE05E
#define	POWER_BREAK_CODE	(0xE05E + BREAK_SET_CODE)

// 2th line
#define	PRINTSCREEN_MAKE_CODE	0xE02AE037
#define	PRINTSCREEN_BREAK_CODE	(0xE0B7E03A + BREAK_SET_CODE)

#define	SCROLL_MAKE_CODE	0x46
#define	SCROLL_BREAK_CODE	(0x46 + BREAK_SET_CODE)

#define	PAUSE_MAKE_CODE		0xE1			// simpler

// 3th line
#define	INSERT_MAKE_CODE	0xE052
#define	INSERT_BREAK_CODE	(0xE052 + BREAK_SET_CODE)

#define	HOME_MAKE_CODE		0xE047
#define	HOME_BREAK_CODE		(0xE047 + BREAK_SET_CODE)

#define	PAGEUP_MAKE_CODE	0xE049
#define	PAGEUP_BREAK_CODE	(0xE049 + BREAK_SET_CODE)

// 4th line
#define	DELETE_MAKE_CODE	0xE053
#define	DELETE_BREAK_CODE	(0xE053 + BREAK_SET_CODE)

#define	END_MAKE_CODE		0xE04F
#define	END_BREAK_CODE		(0xE04F + BREAK_SET_CODE)

#define	PAGEDOWN_MAKE_CODE	0xE051
#define	PAGEDOWN_BREAK_CODE	(0xE051 + BREAK_SET_CODE)

// 5th line
#define	ARROWUP_MAKE_CODE	0xE048
#define	ARROWUP_BREAK_CODE	(0xE048 + BREAK_SET_CODE)

// 6th line
#define	ARROWLEFT_MAKE_CODE	0xE04B
#define	ARROWLEFT_BREAK_CODE	(0xE04B + BREAK_SET_CODE)

#define	ARROWDOWN_MAKE_CODE	0xE050
#define	ARROWDOWN_BREAK_CODE	(0xE050 + BREAK_SET_CODE)

#define	ARROWRIGHT_MAKE_CODE	0xE04D
#define	ARROWRIGHT_BREAK_CODE	(0xE04D + BREAK_SET_CODE)

//-------------------------------------------------------------------------
// key pad
//-------------------------------------------------------------------------
// 1th line
#define	KP_NUM_MAKE_CODE	0x45
#define	KP_NUM_BREAK_CODE	(0x45 + BREAK_SET_CODE)

#define	KP_DIV_MAKE_CODE	0xE035
#define	KP_DIV_BREAK_CODE	(0xE035 + BREAK_SET_CODE)

#define	KP_MUL_MAKE_CODE	0x37
#define	KP_MUL_BREAK_CODE	(0x37 + BREAK_SET_CODE)

#define	KP_SUB_MAKE_CODE	0x4A
#define	KP_SUB_BREAK_CODE	(0x4A + BREAK_SET_CODE)

// 2th line
#define	KP_7_MAKE_CODE		0x47
#define	KP_7_BREAK_CODE		(0x47 + BREAK_SET_CODE)

#define	KP_8_MAKE_CODE		0x48
#define	KP_8_BREAK_CODE		(0x48 + BREAK_SET_CODE)

#define	KP_9_MAKE_CODE		0x49
#define	KP_9_BREAK_CODE		(0x49 + BREAK_SET_CODE)

// 3th line
#define	KP_4_MAKE_CODE		0x4B
#define	KP_4_BREAK_CODE		(0x4B + BREAK_SET_CODE)

#define	KP_5_MAKE_CODE		0x4C
#define	KP_5_BREAK_CODE		(0x4C + BREAK_SET_CODE)

#define	KP_6_MAKE_CODE		0x4D
#define	KP_6_BREAK_CODE		(0x4D + BREAK_SET_CODE)

// 4th line
#define	KP_ADD_MAKE_CODE	0x4E
#define	KP_ADD_BREAK_CODE	(0x4E + BREAK_SET_CODE)

#define	KP_1_MAKE_CODE		0x4F
#define	KP_1_BREAK_CODE		(0x4F + BREAK_SET_CODE)

#define	KP_2_MAKE_CODE		0x50
#define	KP_2_BREAK_CODE		(0x50 + BREAK_SET_CODE)

#define	KP_3_MAKE_CODE		0x51
#define	KP_3_BREAK_CODE		(0x51 + BREAK_SET_CODE)

// 5th line
#define	KP_0_MAKE_CODE		0x52
#define	KP_0_BREAK_CODE		(0x52 + BREAK_SET_CODE)

#define	KP_DOT_MAKE_CODE	0x53
#define	KP_DOT_BREAK_CODE	(0x53 + BREAK_SET_CODE)

#define	KP_ENTER_MAKE_CODE	0xE01C
#define	KP_ENTER_BREAK_CODE	(0xE01C + BREAK_SET_CODE)

//-------------------------------------------------------------------------
// ASCII Code(visibal)
//-------------------------------------------------------------------------
#define	ASC_SPACE		0x20
#define	ASC_EXCLA		0x21
#define	ASC_DBL_QUOTA		0x22
#define	ASC_POUND		0x23
#define	ASC_DOLLAR		0x24
#define	ASC_PERCENT		0x25
#define	ASC_AND			0x26
#define	ASC_E_COMMA		0x27
#define	ASC_L_RBLACKET		0x28
#define	ASC_R_RBLACKET		0x29
#define	ASC_MUL			0x2A
#define	ASC_PLUS		0x2B
#define	ASC_COMMA		0x2C
#define	ASC_MINUS		0x2D
#define	ASC_DOT			0x2E
#define	ASC_DIV			0x2F
#define	ASC_0			0x30
#define	ASC_1			0x31
#define	ASC_2			0x32
#define	ASC_3			0x33
#define	ASC_4			0x34
#define	ASC_5			0x35
#define	ASC_6			0x36
#define	ASC_7			0x37
#define	ASC_8			0x38
#define	ASC_9			0x39
#define	ASC_COLON		0x3A
#define	ASC_SEMICOLON		0x3B
#define	ASC_BLOW		0x3C
#define	ASC_EQUAL		0x3D
#define	ASC_ABOVE		0x3E
#define	ASC_QUESTION		0x3F
#define	ASC_AT			0x40
#define	ASC_A			0x41
#define	ASC_B			0x42
#define	ASC_C			0x43
#define	ASC_D			0x44
#define	ASC_E			0x45
#define	ASC_F			0x46
#define	ASC_G			0x47
#define	ASC_H			0x48
#define	ASC_I			0x49
#define	ASC_J			0x4A
#define	ASC_K			0x4B
#define	ASC_L			0x4C
#define	ASC_M			0x4D
#define	ASC_N			0x4E
#define	ASC_O			0x4F
#define	ASC_P			0x50
#define	ASC_Q			0x51
#define	ASC_R			0x52
#define	ASC_S			0x53
#define	ASC_T			0x54
#define	ASC_U			0x55
#define	ASC_V			0x56
#define	ASC_W			0x57
#define	ASC_X			0x58
#define	ASC_Y			0x59
#define	ASC_Z			0x5A
#define	ASC_L_SBRACKET		0x5B
#define	ASC_E_SLASH		0x5C
#define	ASC_R_SBRACKET		0x5D
#define	ASC_TSQUARE		0x5E
#define	ASC_UNDERLINE		0x5F
#define	ASC_SINGLE_QUOTA	0x60
#define	ASC_LCASE_A		0x61
#define	ASC_LCASE_B		0x62
#define	ASC_LCASE_C		0x63
#define	ASC_LCASE_D		0x64
#define	ASC_LCASE_E		0x65
#define	ASC_LCASE_F		0x66
#define	ASC_LCASE_G		0x67
#define	ASC_LCASE_H		0x68
#define	ASC_LCASE_I		0x69
#define	ASC_LCASE_J		0x6A
#define	ASC_LCASE_K		0x6B
#define	ASC_LCASE_L		0x6C
#define	ASC_LCASE_M		0x6D
#define	ASC_LCASE_N		0x6E
#define	ASC_LCASE_O		0x6F
#define	ASC_LCASE_P		0x70
#define	ASC_LCASE_Q		0x71
#define	ASC_LCASE_R		0x72
#define	ASC_LCASE_S		0x73
#define	ASC_LCASE_T		0x74
#define	ASC_LCASE_U		0x75
#define	ASC_LCASE_V		0x76
#define	ASC_LCASE_W		0x77
#define	ASC_LCASE_X		0x78
#define	ASC_LCASE_Y		0x79
#define	ASC_LCASE_Z		0x7A
#define	ASC_L_BLACKET		0x7B
#define	ASC_V_LINE		0x7C
#define	ASC_R_BLACKET		0x7D
#define	ASC_NOT			0x7E

//-------------------------------------------------------------------------
// keyboard input buffer
//------------------------------------------------------------------------- 

struct keyb_buf {
	char*	write_ptr;
	char*	read_ptr;			
	int	cur_tty;		
	int	count;			
	char	buffer[KB_INPUT_SIZE];	
};

#endif
