//===========================================================================
// fs.c
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
#include	"kernel.h"
#include	"global.h"
#include	"partition.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"prototype.h"


//variable
PRIVATE struct s_dts_super_blk 	super_blk;
PRIVATE struct s_dts_inode	inode;
PRIVATE struct s_dts_dir_ent	dir_ent;

PRIVATE s16 tmp_fsbuf[BLOCK_SIZE/2];

PRIVATE fs_op	dtsfs_tab[NUM_FS_OPERATION] = { dts_add_inode, dts_add_block, dts_add_imap_bit, 
						dts_add_bmap_bit, dts_add_dir_ent, dts_del_inode, 
						dts_del_block, dts_del_imap_bit, dts_del_bmap_bit,
						dts_del_dir_ent, dts_get_inode, dts_get_super_blk,
						dts_update_inode };


PRIVATE	s32	dts_get_inode_seq();
PRIVATE	void	dts_empty_inode(u32 start_addr);
PRIVATE	void	dts_empty_block(u32 start_addr);
PRIVATE	void	dts_empty_ent(u32 start_addr);
PRIVATE	void	dts_empty_buf(u32 start_addr);


//===========================================================================
// get fs_tab
//===========================================================================

PUBLIC	fs_op dts_get_fstab()
{
	return dtsfs_tab;
}


//===========================================================================
// get inode
//===========================================================================
PUBLIC	u32 	dts_get_inode(s32 dev, u8 * ent_name, struct s_dts_inode * src_inode)
{
	s8	file[DIR_ENT_NAME_LEN];
	s8	tmp_ent[DIR_ENT_NAME_LEN];

	//-----------------------------------------------------------------
	// init
	//-----------------------------------------------------------------
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;
	
	// inode_start_addr = 0x4F4F * 0x200 = 0x9E9E00
	u32	inode_start_addr= inode_start_sec * SECTOR_SIZE;	

	// data_start_sec   = 0x4EFF + (0 + 1 + 2 + 6 + 1 + 0x200) * 8 = 0x5F4F
	u32	data_start_sec = part->data_start_sec;

	// data_start_addr = 0x5F4F * 0x200 = 0xBE9E00
	u32	data_start_addr	= data_start_sec * SECTOR_SIZE;
	
	// how many inodes in a block
	u32	nr_inode_per_block = vfs->spk.block_size / vfs->spk.inode_size;
	
	u32	inode_size	= vfs->spk.inode_size;


	//-----------------------------------------------------------------
	// loop to search each dir
	//-----------------------------------------------------------------
	s8 flg_dir = FALSE;
	struct s_v_dir_ent * v_dir_ent;
	struct s_v_inode   * v_inode;
	s8 *p_dir = ent_name;
	u32	inode_num;
	u32	blk, offset;
	u32	start_sec;

	// step 1: read root dir into tmp_fsbuf
	s32 i = 0;	
	
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
	mesg.msg.parm1	= data_start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	//-----------------------------------------------------------------
	// root dir processing(add on 2012-11-5)
	//-----------------------------------------------------------------
	if(strcmp("//", ent_name))
	{
		inode_num	= 1;

		blk		= (inode_num - 1) / nr_inode_per_block;
		offset		= (inode_num - 1) % nr_inode_per_block;
		
		start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);	

		
		v_inode = &tmp_fsbuf[offset * inode_size / 2];

		strcpy(src_inode, v_inode, sizeof(struct s_v_inode));

		start_sec	= v_inode->i_block_0;		

		
		struct s_mesg mesg1;

		mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
		mesg1.msg.parm1	= start_sec;
		mesg1.msg.parm2	= 1;			// read 1 sector
		mesg1.src_pid	= PROC_FS;
		mesg1.dest_pid	= PROC_HD;
		mesg1.type	= HARDDISK_READ;
		mesg1.func	= SYN;
		
		send_msg(&mesg1);

		return	ENTRY_EXIST;
	}

	//-----------------------------------------------------------------
	// other dir processing
	//-----------------------------------------------------------------
	v_dir_ent = &tmp_fsbuf;
	tmp_ent[i] = '/';
	i++;
	
	// step 3: compare each part of dir with the entry existed in tmp_fsbuf
	for(; *p_dir != NULL ; p_dir++)
	{
		if(*p_dir != '/') 
		{
			tmp_ent[i] = *p_dir;
			i++;
			continue;
		}
		tmp_ent[i] = '\0';

		while(v_dir_ent->dir_inode != 0)
		{
			flg_dir = strcmp(tmp_ent, v_dir_ent->dir_name);

			if(flg_dir == TRUE)
			{
				break;
			}

			v_dir_ent++;
		}	

		if(flg_dir == FALSE)
		{
			return DIR_NOT_FOUND;
		}

		inode_num	= v_dir_ent->dir_inode;

		blk		= (inode_num - 1) / nr_inode_per_block;
		offset		= (inode_num - 1) % nr_inode_per_block;
		
		start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);
		
		v_inode = &tmp_fsbuf[offset * inode_size / 2];

		strcpy(src_inode, v_inode, sizeof(struct s_v_inode));

		start_sec	= v_inode->i_block_0;
		
		struct s_mesg mesg1;

		mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
		mesg1.msg.parm1	= start_sec;
		mesg1.msg.parm2	= 1;			// read 1 sector
		mesg1.src_pid	= PROC_FS;
		mesg1.dest_pid	= PROC_HD;
		mesg1.type	= HARDDISK_READ;
		mesg1.func	= SYN;
		
		send_msg(&mesg1);

		v_dir_ent = &tmp_fsbuf;

		i = 0;
		flg_dir = FALSE;
	}

	return	ENTRY_EXIST;
}

//===========================================================================
// get_super_block
//===========================================================================
PUBLIC	struct s_dts_super_blk * dts_get_super_blk(s32 dev)
{
	struct s_dts_super_blk * psuperblk = &super_blk;

	// the 0th block is boot block while 1th is the super block; 
	// so need to and 1 block(1 block = 8 sector) 
	u32	start_sec = LOG_SEC_START + SECTOR_PER_BLOCK; 		// 0x4EFF + 8 = 0x4F07

	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	psuperblk = &tmp_fsbuf;

	return psuperblk;
}

//===========================================================================
// add inode
//===========================================================================
PUBLIC	void	dts_add_inode(s32 dev, s32 inode_seq_num)
{
	struct s_dts_inode   * inode;

	struct s_dts_super_blk * sblk	= dts_get_super_blk(dev);
	
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;

	// how many inodes in a block
	u32	nr_inode_per_block = sblk->block_size / sblk->inode_size;
	
	u32	blk		= (inode_seq_num - 1) / nr_inode_per_block;
	u32	offset		= (inode_seq_num - 1) % nr_inode_per_block;
		
	u32	start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	inode = (fsbuf + offset * sblk->inode_size);

	// setting inode mode
	inode->i_mode = INODE_R_W_E_MODE;

	// setting initial size of an inode
	inode->i_size = 0;

	// setting inode number
	inode->i_num = inode_seq_num;

	// update inode	
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);
}


//===========================================================================
// add block
//===========================================================================
PUBLIC	void	dts_add_block(s32 dev, s32 inode_seq_num, u32 size_in_blk)
{
	struct s_dts_inode   * inode;

	struct s_dts_super_blk * sblk	= dts_get_super_blk(dev);
	
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;

	// how many inodes in a block
	u32	nr_inode_per_block = sblk->block_size / sblk->inode_size;

	u32	block_size	= sblk->block_size;
	
	u32	blk		= (inode_seq_num - 1) / nr_inode_per_block;
	u32	offset		= (inode_seq_num - 1) % nr_inode_per_block;
		
	u32	start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);	

	inode = (fsbuf + offset * sblk->inode_size);	

	if(size_in_blk <= 6)
	{
		int i;
		for(i=0; i<size_in_blk; i++)
		{
			switch(i)
			{
				case 0:
					inode->i_block_0 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
				case 1:
					inode->i_block_1 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
				case 2:
					inode->i_block_2 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
				case 3:
					inode->i_block_3 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
				case 4:
					inode->i_block_4 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
				case 5:
					inode->i_block_5 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					break;
			}
		}
	}
	else if(size_in_blk <= 1024)	// 0 ~ 1024
	{
		u32 start_blk		= alloc_block(1) ;	
		inode->i_block_1lvl_ind = start_blk * SECTOR_PER_BLOCK / block_size; 

		s32 i;
		s32 count = size_in_blk ;
		s32 * p = (s32 *)(start_blk * sblk->block_size);

		for(i=0; i<count; i++, p++)
		{
			*p = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
		}
	}

	// update inode	
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);	
}


//===========================================================================
// add imap bit
//===========================================================================
PUBLIC	void	dts_add_imap_bit(s32 dev)
{
	s16 * fsb = fsbuf;
	//s16 * fsb = p_fsbuf;

	// inode_start_sec = 0x4EFF + (0 + 1 + 1) * 8 = 0x4F0F
	u32	imap_start_sec	= part->imap_start_sec;

	//u32	imap_end_sec	= LOG_SEC_START + (0 + 1 + nr_imap_blks + 1) * SECTOR_PER_BLOCK;
	u32	imap_end_sec	= imap_start_sec + part->imap_size_in_sec;
	
	// inode_start_addr = 0x4F4F * 0x200 = 0x9E1E00
	u32	imap_start_addr	= imap_start_sec * SECTOR_SIZE;

	u32 sec;
	for(sec = imap_start_sec; sec < imap_end_sec; sec = sec + SECTOR_PER_BLOCK)
	{		
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);		

		if(fsb[2047] != 0xFFFF)
		{
			break;
		}
	}

	s32 i;
	u16 tv;
	for(i = 0; i < 2048; i++)
	{
		tv = fsb[i];

		if(tv == 0xFFFE)
		{
			if((i == 0) && (sec == imap_start_sec))
			{
				continue;
			}
		}

		if(tv != 0xFFFF)
		{
			break;
		}
	}

	u8 tmp = num2bit(fsb[i]);

	if(tmp == 16)
	{
		if(i == 2047)
		{
			// save the current data
			struct s_mesg mesg;

			mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
			mesg.msg.parm1	= sec;
			mesg.msg.parm2	= 1;			// read 1 sector
			mesg.src_pid	= PROC_FS;
			mesg.dest_pid	= PROC_HD;
			mesg.type	= HARDDISK_WRITE;
			mesg.func	= SYN;
		
			send_msg(&mesg);			

			sec = sec + SECTOR_PER_BLOCK;

			if(sec >= imap_end_sec)
			{			
				printk("err: imap overload\n");

				return 1;
			} 

			struct s_mesg mesg1;

			mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
			mesg1.msg.parm1	= sec;
			mesg1.msg.parm2	= 1;			// read 1 sector
			mesg1.src_pid	= PROC_FS;
			mesg1.dest_pid	= PROC_HD;
			mesg1.type	= HARDDISK_READ;
			mesg1.func	= SYN;
		
			send_msg(&mesg1);

			i = 0;

			fsb[i] = 1;
		} 
		else
		{
			i++;
			fsb[i] = 1;
		}
	}
	else
	{
		tmp++;
		fsb[i] = bit2num(tmp);
	}

	// update
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_WRITE;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}

//===========================================================================
// add bmap bit
//===========================================================================
PUBLIC	void	dts_add_bmap_bit(s32 dev, s32 size_in_blk)
{
	s16 * fsb = fsbuf;	
	
	u32	bmap_start_sec	= part->bmap_start_sec;

	u32	bmap_end_sec	= bmap_start_sec + part->bmap_size_in_sec;
	
	u32 sec;
	for(sec = bmap_start_sec; sec < bmap_end_sec; sec = sec + SECTOR_PER_BLOCK)
	{
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);		

		if(fsb[2047] != 0xFFFF)
		{
			break;
		}
	}

	s32 i;
	u16 tv;
	for(i = 0; i < 2048; i++)
	{
		tv = fsb[i];

		if(tv == 0xFFFE)
		{
			if((i == 0) && (sec == bmap_start_sec))
			{
				continue;
			}
		}

		if(tv != 0xFFFF)
		{
			break;
		}
	}

	u8 tmp;

	s32 j;
	for(j=0; j<size_in_blk; j++)
	{
		tmp = num2bit(fsb[i]);
		tmp++;
		fsb[i] = bit2num(tmp);

		if(fsb[i] == 0xFFFF)
		{
			i++;
			
			if(i == 2048)
			{
				// save the current data
				struct s_mesg mesg;

				mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
				mesg.msg.parm1	= sec;
				mesg.msg.parm2	= 1;			// read 1 sector
				mesg.src_pid	= PROC_FS;
				mesg.dest_pid	= PROC_HD;
				mesg.type	= HARDDISK_WRITE;
				mesg.func	= SYN;
		
				send_msg(&mesg);				

				sec = sec + SECTOR_PER_BLOCK;

				if(sec >= bmap_end_sec)
				{					
					printk("err: bmap overload\n");

					return 1;
				} 

				struct s_mesg mesg1;

				mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
				mesg1.msg.parm1	= sec;
				mesg1.msg.parm2	= 1;			// read 1 sector
				mesg1.src_pid	= PROC_FS;
				mesg1.dest_pid	= PROC_HD;
				mesg1.type	= HARDDISK_READ;
				mesg1.func	= SYN;
		
				send_msg(&mesg1);

				i = 0;
			}
		}
	}

	// update
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_WRITE;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}

//===========================================================================
// add entry
//===========================================================================
PUBLIC	s32	dts_add_dir_ent(s32 dev, u32 entry_start_sec, s8 * name)
{
	struct s_dts_dir_ent * dir_ent;

	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= entry_start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	for(dir_ent = &fsbuf; dir_ent->dir_inode != 0; dir_ent++)
	{}

	s32	inode_seq_num	= dts_get_inode_seq() + 1;
	dir_ent->dir_inode	= inode_seq_num;
	strcopy(dir_ent->dir_name, name);

	// update inode	
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= entry_start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);

	return	inode_seq_num;
}


//===========================================================================
// delete inode
//===========================================================================
PUBLIC	void	dts_del_inode(s32 dev, s32 inode_seq_num)
{
	struct s_dts_inode   * inode;

	struct s_dts_super_blk * sblk	= dts_get_super_blk(dev);
	
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;

	// how many inodes in a block
	u32	nr_inode_per_block = sblk->block_size / sblk->inode_size;
	
	u32	blk		= (inode_seq_num - 1) / nr_inode_per_block;
	u32	offset		= (inode_seq_num - 1) % nr_inode_per_block;
		
	u32	start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	inode = (fsbuf + offset * sblk->inode_size);

	// empty an inode
	dts_empty_inode(inode);
	
	// update inode	
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);
}


//===========================================================================
// delete block
//===========================================================================
PUBLIC	void	dts_del_block(s32 dev, s32 inode_seq_num, u32 size_in_blk)
{
	struct s_dts_inode   * inode;

	struct s_dts_super_blk * sblk	= dts_get_super_blk(dev);
	
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;

	// how many inodes in a block
	u32	nr_inode_per_block = sblk->block_size / sblk->inode_size;

	u32	block_size	= sblk->block_size;
	
	u32	blk		= (inode_seq_num - 1) / nr_inode_per_block;
	u32	offset		= (inode_seq_num - 1) % nr_inode_per_block;
		
	u32	start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	inode = (fsbuf + offset * sblk->inode_size);

	u32	addr;

	if(size_in_blk <= 6)
	{
		int i;
		for(i=0; i<size_in_blk; i++)
		{
			switch(i)
			{
				case 0:
					addr = inode->i_block_0 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
				case 1:
					addr = inode->i_block_1 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
				case 2:
					addr = inode->i_block_2 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
				case 3:
					addr = inode->i_block_3 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
				case 4:
					addr = inode->i_block_4 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
				case 5:
					addr = inode->i_block_5 * block_size / SECTOR_PER_BLOCK;
					free_block(addr, 1);
					// empty an block
					dts_empty_block(addr);
					break;
			}
		}
	}
	else if(size_in_blk <= 1024)	// 0 ~ 1024
	{
		s32 i;
		s32 count = size_in_blk ;
		s32 * p = (s32 *)(inode->i_block_1lvl_ind);

		for(i=0; i<count; i++, p++)
		{
			addr = (*p) * block_size / SECTOR_PER_BLOCK;
			free_block(addr, 1);
			// empty an block
			dts_empty_block(addr);
		}
	}

	// update inode		
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);	
}




//===========================================================================
// delete imap bit
//===========================================================================
PUBLIC	void	dts_del_imap_bit(s32 dev)
{
	s16 * fsb = fsbuf;
	//s16 * fsb = p_fsbuf;

	// inode_start_sec = 0x4EFF + (0 + 1 + 1) * 8 = 0x4F0F
	u32	imap_start_sec	= part->imap_start_sec;

	//u32	imap_end_sec	= LOG_SEC_START + (0 + 1 + nr_imap_blks + 1) * SECTOR_PER_BLOCK;
	u32	imap_end_sec	= imap_start_sec + part->imap_size_in_sec;
	
	// inode_start_addr = 0x4F4F * 0x200 = 0x9E1E00
	u32	imap_start_addr	= imap_start_sec * SECTOR_SIZE;

	u32 sec;
	for(sec = imap_start_sec; sec < imap_end_sec; sec = sec + SECTOR_PER_BLOCK)
	{
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);

		if(fsb[2047] != 0xFFFF)
		{
			break;
		}
	}

	s32 i;
	u16 tv;
	for(i = 0; i < 2048; i++)
	{
		tv = fsb[i];

		if(tv == 0xFFFE)
		{
			if((i == 0) && (sec == imap_start_sec))
			{
				continue;
			}
		}

		if(tv != 0xFFFF)
		{
			break;
		}
	}		

	u8 tmp = num2bit(fsb[i]);

	if(tmp == 1)
	{
		if((i == 0) && (sec == imap_start_sec))
		{
			printk("err: bmap overload\n");
			return 1;
		}
		else
		{
			i--;
			fsb[i] = 0xFFFF;
		} 
	}
	else if(tmp == 0)
	{
		// save the current data
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_WRITE;
		mesg.func	= SYN;
		
		send_msg(&mesg);

		sec = sec - SECTOR_PER_BLOCK;

		if(sec <= imap_start_sec)
		{
			printk("err: imap overload\n");
			return 1;
		}

		struct s_mesg mesg1;

		mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg1.msg.parm1	= sec;
		mesg1.msg.parm2	= 1;			// read 1 sector
		mesg1.src_pid	= PROC_FS;
		mesg1.dest_pid	= PROC_HD;
		mesg1.type	= HARDDISK_READ;
		mesg1.func	= SYN;
		
		send_msg(&mesg1);

		i = 2047;

		fsb[i] = 0xFFFE;
	}
	else
	{
		tmp--;
		fsb[i] = bit2num(tmp);
	}

	// update	
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_WRITE;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}


//===========================================================================
// delete bmap bit
//===========================================================================
PUBLIC	void	dts_del_bmap_bit(s32 dev, s32 size_in_blk)
{
	s16 * fsb = fsbuf;	
	
	u32	bmap_start_sec	= part->bmap_start_sec;

	u32	bmap_end_sec	= bmap_start_sec + part->bmap_size_in_sec;
	
	u32 sec;
	for(sec = bmap_start_sec; sec < bmap_end_sec; sec = sec + SECTOR_PER_BLOCK)
	{
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);

		if(fsb[2047] != 0xFFFF)
		{
			break;
		}
	}

	s32 i;
	u16 tv;
	for(i = 0; i < 2048; i++)
	{
		tv = fsb[i];

		if(tv == 0xFFFE)
		{
			if((i == 0) && (sec == bmap_start_sec))
			{
				continue;
			}
		}

		if(tv != 0xFFFF)
		{
			break;
		}
	}

	u8 tmp;

	s32 j;
	for(j=0; j<size_in_blk; j++)
	{
		tmp = num2bit(fsb[i]);
		tmp--;
		fsb[i] = bit2num(tmp);

		if(fsb[i] == 0)
		{
			i--;
			
			if(i == 0)
			{
				// save the current data				
				struct s_mesg mesg;

				mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
				mesg.msg.parm1	= sec;
				mesg.msg.parm2	= 1;			// read 1 sector
				mesg.src_pid	= PROC_FS;
				mesg.dest_pid	= PROC_HD;
				mesg.type	= HARDDISK_WRITE;
				mesg.func	= SYN;
		
				send_msg(&mesg);			

				sec = sec - SECTOR_PER_BLOCK;

				if(sec <= bmap_start_sec)
				{					
					printk("err: bmap overload\n");
					return 1;
				} 

				struct s_mesg mesg1;

				mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
				mesg1.msg.parm1	= sec;
				mesg1.msg.parm2	= 1;			// read 1 sector
				mesg1.src_pid	= PROC_FS;
				mesg1.dest_pid	= PROC_HD;
				mesg1.type	= HARDDISK_READ;
				mesg1.func	= SYN;
		
				send_msg(&mesg1);

				i = 2047;
			}
		}
	}

	// update
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_WRITE;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}

//===========================================================================
// delete entry
//===========================================================================
PUBLIC	s32	dts_del_dir_ent(s32 dev, u32 entry_start_sec, s8 * name)
{
	struct s_dts_dir_ent * dir_ent;
	s8 flg = FALSE;

	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= entry_start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);	

	for(dir_ent = &fsbuf; dir_ent->dir_inode != 0; dir_ent++)	
	{
		flg = strcmp(name, dir_ent->dir_name);

		if(flg == TRUE)
		{
			break;
		}	
	}

	s32	inode_seq_num	= dir_ent->dir_inode;
	
	// empty an entry
	dts_empty_ent(dir_ent);

	// update inode	
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= entry_start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);	

	return	inode_seq_num;
}



//===========================================================================
// update inode
//===========================================================================
PUBLIC	void	dts_update_inode(s32 dev, struct s_dts_inode * src_inode)
{
	struct s_dts_inode   * inode;

	struct s_dts_super_blk * sblk	= dts_get_super_blk(dev);

	u32	inode_seq_num 	= src_inode->i_num;
	
	// inode_start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F
	u32	inode_start_sec	= part->inode_start_sec;

	// how many inodes in a block
	u32	nr_inode_per_block = sblk->block_size / sblk->inode_size;
	
	u32	blk		= (inode_seq_num - 1) / nr_inode_per_block;
	u32	offset		= (inode_seq_num - 1) % nr_inode_per_block;
		
	u32	start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;
 
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);	

	inode = (fsbuf + offset * sblk->inode_size);
	
	// update inode
	strcpy(inode, src_inode, sblk->inode_size);

	// update inode
	struct s_mesg mesg1;

	mesg1.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
	mesg1.msg.parm1	= start_sec;
	mesg1.msg.parm2	= 1;			// read 1 sector
	mesg1.src_pid	= PROC_FS;
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HARDDISK_WRITE;
	mesg1.func	= SYN;
		
	send_msg(&mesg1);	
}

//===========================================================================
// get inode sequence number
//===========================================================================
PRIVATE	s32	dts_get_inode_seq()
{
	//s16 * tmp_bf = tmp_fsbuf;

	// inode_start_sec = 0x4EFF + (0 + 1 + 1) * 8 = 0x4F0F
	u32	imap_start_sec	= part->imap_start_sec;

	//u32	imap_end_sec	= LOG_SEC_START + (0 + 1 + nr_imap_blks + 1) * SECTOR_PER_BLOCK;
	u32	imap_end_sec	= imap_start_sec + part->imap_size_in_sec;
	
	// inode_start_addr = 0x4F4F * 0x200 = 0x9E1E00
	u32	imap_start_addr	= imap_start_sec * SECTOR_SIZE;

	u32 sec;
	for(sec = imap_start_sec; sec < imap_end_sec; sec = sec + SECTOR_PER_BLOCK)
	{
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
		mesg.msg.parm1	= sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);

		if(tmp_fsbuf[2047] != 0xFFFF)
		{
			break;
		}
	}

	s32 i;
	for(i = 0; i < 2048; i++)
	{
		u16 tv = tmp_fsbuf[i];

		if(tv == 0xFFFE)
		{
			if((i == 0) && (sec == imap_start_sec))
			{
				continue;
			}
		}

		if(tv != 0xFFFF)
		{
			break;
		}
	}

	u32 blk, sum, offset;

	sec = sec - imap_start_sec;
	blk = sec / SECTOR_PER_BLOCK;	
	sum = blk * (8 * 4096) ;

	offset = i * 16 + num2bit(tmp_fsbuf[i]);
	sum = sum + offset;

	return sum;
}



//===========================================================================
// fill '\0' to a block
//===========================================================================
PRIVATE	void	dts_empty_block(u32 start_addr)
{
	u32 size= vfs->spk.block_size;
	s8 *p	= (s8*)start_addr;

	u32 i;
	for(i=0; i < size; i++)
	{
		p[i] = '\0';
	} 
}

//===========================================================================
// fill '\0' to a inode
//===========================================================================
PRIVATE	void	dts_empty_inode(u32 start_addr)
{
	u32 size= vfs->spk.inode_size;
	s8 *p	= (s8*)start_addr;

	u32 i;
	for(i=0; i < size; i++)
	{
		p[i] = '\0';
	} 
}

//===========================================================================
// fill '\0' to an entry
//===========================================================================
PRIVATE	void	dts_empty_ent(u32 start_addr)
{
	u32 size= DIR_ENT_SIZE;
	s8 *p	= (s8*)start_addr;

	u32 i;
	for(i=0; i < size; i++)
	{
		p[i] = '\0';
	} 
}


//===========================================================================
// fill '\0' to an buffer
//===========================================================================
PRIVATE	void	dts_empty_buf(u32 start_addr)
{
	u32 size= BLOCK_SIZE/2;
	s8 *p	= (s8*)start_addr;

	u32 i;
	for(i=0; i < size; i++)
	{
		p[i] = '\0';
	} 
}
