//===========================================================================
// time.c
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
#include	"stdio.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"mesg.h"
#include	"sys.h"
#include	"time.h"
#include	"prototype.h"

extern	u32	ticket;

PRIVATE	struct s_time	sys_rtime_s;	//is the time that the system start to run
PRIVATE	struct s_time	sys_ntime_s;	//now

PRIVATE u32 simp_year[13][2]= 	{{0,0},  {31, 0}, {28, 0}, {31, 0}, {30, 0}, {31, 0}, {30, 0}, {31, 0}, 
				{31, 0}, {30, 0}, {31, 0}, {30, 0}, {31, 0}};

PRIVATE u32 leap_year[13][2]= 	{{0,0},  {31, 0}, {29, 0}, {31, 0}, {30, 0}, {31, 0}, {30, 0}, {31, 0}, 
				{31, 0}, {30, 0}, {31, 0}, {30, 0}, {31, 0}};

PRIVATE u32 day[32][2]= 	{{0,0},  {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8,0},  {9, 0}, 
				 {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},	 {16,0}, {17, 0},{18, 0}, {19, 0}, 
				 {20, 0}, {21, 0}, {22, 0}, {23, 0}, {24, 0}, {25, 0}, {26, 0}, {27, 0}, {28, 0}, {29, 0},
				 {30, 0}, {31, 0}} ;

PRIVATE u32 hour[24][2]= 	{{0,0},  {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, 
				 {8,0},  {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0}, 
				 {16,0}, {17, 0},{18, 0}, {19, 0}, {20, 0}, {21, 0}, {22, 0}, {23, 0}} ;

PRIVATE u32 min[60][2]= 	{{0,0},  {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8,0},  {9, 0}, 
				 {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},	 {16,0}, {17, 0},{18, 0}, {19, 0}, 
				 {20, 0}, {21, 0}, {22, 0}, {23, 0}, {24, 0}, {25, 0}, {26, 0}, {27, 0}, {28, 0}, {29, 0},
				 {30, 0}, {31, 0}, {32, 0}, {33, 0}, {34, 0}, {35, 0}, {36, 0}, {37, 0}, {38, 0}, {39, 0},
				 {40, 0}, {41, 0}, {42, 0}, {43, 0}, {44, 0}, {45, 0}, {46, 0}, {47, 0}, {48, 0}, {49, 0},
				 {50, 0}, {51, 0}, {52, 0}, {53, 0}, {54, 0}, {55, 0}, {56, 0}, {57, 0}, {58, 0}, {59, 0}} ;


PRIVATE u32 year_sum[100][2];
//===========================================================================
//systime_init()
//===========================================================================
PUBLIC	void	systime_init()
{
	sys_rtime	= &sys_rtime_s;
	sys_ntime	= &sys_ntime_s;

	u8 cent_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_CENT);
	u8 year_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_YEAR);
	u8 mon_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_MON);
	u8 day_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_DAY);
	u8 hour_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_HOUR);
	u8 min_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_MIN);
	u8 sec_t	= *(u8*)(CMOS_INFO_ADDR + DATE_OFF_SEC);

	sys_rtime->year	= bcd_2_i(cent_t) * 100 + bcd_2_i(year_t);
	sys_rtime->mon	= bcd_2_i(mon_t);
	sys_rtime->day	= bcd_2_i(day_t);
	sys_rtime->hour	= bcd_2_i(hour_t);
	sys_rtime->min	= bcd_2_i(min_t);
	sys_rtime->sec	= bcd_2_i(sec_t);

	sys_ntime->year	= sys_rtime->year;
	sys_ntime->mon	= sys_rtime->mon;
	sys_ntime->day	= sys_rtime->day;
	sys_ntime->hour	= sys_rtime->hour;
	sys_ntime->min	= sys_rtime->min;
	sys_ntime->sec	= sys_rtime->sec;

	//--------------------------------------------------------------------------------
	// year[0][1] 	= sum_of_2000
	// year[1][1]	= sum_of_2000 + sum_of_2001
	// year[2][1]	= sum_of_2000 + sum_of_2001 + sum_of_2002
	// .....
	// year[99][1]	= sum_of_2000 + ... + sum_of_2099
	//--------------------------------------------------------------------------------
	u32 i,j,sum;
	for(i=2000,j=0,sum=0; j<100; i++,j++)
	{
		if(i % 4)
		{
			sum	+= A_SIMP_YEAR_SECONDS;
		}
		else
		{
			sum	+= A_LEAP_YEAR_SECONDS;
		}

		year_sum[j][0]	= i;
		year_sum[j][1]	= sum;
	}

	//--------------------------------------------------------------------------------
	// year[1][1] 	= sum_of_JAN
	// year[2][1]	= sum_of_JAN + sum_of_FEB
	// year[3][1]	= sum_of_JAN + sum_of_FEB + sum_of_MAR
	// .....
	// year[12][1]	= sum_of_JAN + ... + sum_of_DEC
	//--------------------------------------------------------------------------------
	for(j=1,sum=0; j<13; j++)
	{
		sum	+= simp_year[j][0] * A_DAY_SECONDS;
		simp_year[j][1]	= sum;
	}

	for(j=1,sum=0; j<13; j++)
	{
		sum	+= leap_year[j][0] * A_DAY_SECONDS;
		leap_year[j][1]	= sum;
	}

	//--------------------------------------------------------------------------------
	// day[1][1]	= sum_of_0 + sum_of_1
	// day[2][1]	= sum_of_0 + sum_of_1 + sum_of_2
	// .....
	// day[31][1]	= sum_of_0 + ... + sum_of_31
	//--------------------------------------------------------------------------------
	for(j=1; j<32; j++)
	{
		day[j][1]	= day[j][0] * A_DAY_SECONDS;		
	}

	//--------------------------------------------------------------------------------
	// hour[0][1] 	= sum_of_0
	// hour[1][1]	= sum_of_0 + sum_of_1
	// hour[2][1]	= sum_of_0 + sum_of_1 + sum_of_2
	// .....
	// hour[23][1]	= sum_of_0 + ... + sum_of_23
	//--------------------------------------------------------------------------------
	for(j=0; j<24; j++)
	{
		//modi on 2013-2-13
		//hour[j][1]	= hour[j][0] * A_HOUR_SECONDS;
		hour[j][1]	= (hour[j][0]+1) * A_HOUR_SECONDS;		
	}

	//--------------------------------------------------------------------------------
	// min[0][1] 	= sum_of_0
	// min[1][1]	= sum_of_0 + sum_of_1
	// min[2][1]	= sum_of_0 + sum_of_1 + sum_of_2
	// .....
	// min[59][1]	= sum_of_0 + ... + sum_of_59
	//--------------------------------------------------------------------------------
	for(j=0; j<60; j++)
	{
		//modi on 2013-2-13
		//min[j][1]	= min[j][0] * A_MIN_SECONDS;
		min[j][1]	= (min[j][0]+1) * A_MIN_SECONDS;		
	}

	//--------------------------------------------------------------------------------
	// system run up seconds
	//--------------------------------------------------------------------------------
	sum	= 0;

	if(sys_rtime->year > BASIC_YEAR)
	{
		sum 	= year_sum[sys_rtime->year - BASIC_YEAR - 1][1];
	}

	if(sys_rtime->mon > JAN)
	{
		if(sys_rtime->year % 4)
		{
			sum	+= simp_year[sys_rtime->mon - 1][1];
		}
		else
		{
			sum	+= leap_year[sys_rtime->mon - 1][1];
		}
	}

	if(sys_rtime->day > 1)
	{
		sum	+= day[sys_rtime->day-1][1];
	}

	//if(sys_rtime->hour > 1)
	if(sys_rtime->hour > 0)
	{
		//sum	+= hour[sys_rtime->hour][1];
		sum	+= hour[sys_rtime->hour-1][1];
	}

	//if(sys_rtime->min > 1)
	if(sys_rtime->min > 0)
	{
		//sum	+= min[sys_rtime->min][1];
		sum	+= min[sys_rtime->min-1][1];
	}
	
	sum	+= sys_rtime->sec;

	sysrtime_sec	= sum;
	sysntime_sec	= sysrtime_sec + ticket / TIME_HZ;
}

//===========================================================================
//systime_set()
//===========================================================================
PUBLIC	void	systime_set()
{
	//--------------------------------------------------------------------------------
	// sysntime setting
	//--------------------------------------------------------------------------------
	sysntime_sec	= sysrtime_sec + (ticket / TIME_HZ / 4);	// the reason is unknown

	u32	t	= sysntime_sec;

	//--------------------------------------------------------------------------------
	// year
	//--------------------------------------------------------------------------------
	u32 i;
	for(i=0; i<100; i++)
	{
		if(t < year_sum[i][1])
		{
			break;
		}
	}

	sys_ntime->year	= year_sum[i][0];	//year

	t -= year_sum[i-1][1];

	//--------------------------------------------------------------------------------
	// month
	//--------------------------------------------------------------------------------
	if(sys_ntime->year % 4)
	{
		for(i=1; i<13; i++)
		{
			if(t < simp_year[i][1])
			{
				break;
			}			
		}
		sys_ntime->mon	= i;	//mon
		t -= simp_year[i-1][1];		
	}
	else
	{
		for(i=1; i<13; i++)
		{
			if(t < leap_year[i][1])
			{
				break;
			}			
		}
		sys_ntime->mon	= i;	//mon
		t -= leap_year[i-1][1];
	}
	
	//--------------------------------------------------------------------------------
	// day
	//--------------------------------------------------------------------------------
	for(i=1; i<32; i++)
	{
		if(t < day[i][1])
		{
			break;
		}
	}
	sys_ntime->day	= day[i][0];	//day
	t -= day[i-1][1];

	//--------------------------------------------------------------------------------
	// hour
	//--------------------------------------------------------------------------------
	for(i=0; i<24; i++)
	{
		if(t < hour[i][1])
		{
			break;
		}
	}

	if(i>0)
	{
		sys_ntime->hour	= hour[i][0];	//hour
		t -= hour[i-1][1];
	}
	else
	{
		sys_ntime->hour	= 0;
	}
	

	//--------------------------------------------------------------------------------
	// min
	//--------------------------------------------------------------------------------
	for(i=0; i<60; i++)
	{
		if(t < min[i][1])
		{
			break;
		}
	}

	if(i>0)
	{
		sys_ntime->min	= min[i][0];	//min
		t -= min[i-1][1];
	}
	else
	{
		sys_ntime->min	= 0;
	}

	//--------------------------------------------------------------------------------
	// sec
	//--------------------------------------------------------------------------------
	sys_ntime->sec	= t;	
}


//===========================================================================
//delay_sec(u32 sec)::delay seconds
//===========================================================================
PUBLIC	void	delay_sec(u32 sec)
{
	u32	t = ticket;
	
	while(((ticket-t)/TIME_HZ/4)<sec)	
	{};
}
