//===========================================================================
// vfslib.c
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
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"mesg.h"
#include	"prototype.h"

PRIVATE s8 tmp_fsbuf[BLOCK_SIZE];
//===========================================================================
// k_mkdir/k_mkfile(u32 start_sec, char *dir_name)
//===========================================================================
PUBLIC	void k_mkinode(u32 entry_start_sec, char *dir_name)
{
	struct s_v_dir_ent * v_dir_ent;
	struct s_v_inode   * v_inode;
	s8	file[DIR_ENT_NAME_LEN];
	
	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// init file[]
	//-----------------------------------------------------------------
	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// add entry
	//-----------------------------------------------------------------
	s32 inode_seq_num = vfs->add_dir_ent(dev, entry_start_sec, file);
	
	//-----------------------------------------------------------------
	// add inode
	//-----------------------------------------------------------------
	vfs->add_inode(dev, inode_seq_num);

	//-----------------------------------------------------------------
	// add inode imap
	//-----------------------------------------------------------------
	vfs->add_imap_bit(dev);

	//-----------------------------------------------------------------
	// add block
	//-----------------------------------------------------------------
	vfs->add_block(dev, inode_seq_num, 1);

	//-----------------------------------------------------------------
	// add bmap
	//-----------------------------------------------------------------
	vfs->add_bmap_bit(dev, 1);
}


//===========================================================================
// k_rmdir/k_rmfile(u32 start_sec, char *dir_name)
//===========================================================================
PUBLIC	void k_rminode(u32 entry_start_sec, char *dir_name)
{
	struct s_v_dir_ent * v_dir_ent;
	struct s_v_inode   * v_inode;
	s8	file[DIR_ENT_NAME_LEN];
	
	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// init file[]
	//-----------------------------------------------------------------
	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// delete entry
	//-----------------------------------------------------------------
	s32 inode_seq_num = vfs->del_dir_ent(dev, entry_start_sec, file);
	
	//-----------------------------------------------------------------
	// delete inode
	//-----------------------------------------------------------------
	vfs->del_inode(dev, inode_seq_num);

	//-----------------------------------------------------------------
	// delete inode imap
	//-----------------------------------------------------------------
	vfs->del_imap_bit(dev);

	//-----------------------------------------------------------------
	// delete block
	//-----------------------------------------------------------------
	vfs->del_block(dev, inode_seq_num, 1);

	//-----------------------------------------------------------------
	// delete bmap
	//-----------------------------------------------------------------
	vfs->del_bmap_bit(dev, 1);

	
}

//===========================================================================
// k_fopen(u32 start_sec, char *dir_name)
//===========================================================================
PUBLIC	u32 	k_fopen(s32 dev, u8 * file_name, struct s_dts_inode * src_inode)
{
	s32 	dir_inode_seq = vfs->get_inode(dev, file_name, src_inode);

	return dir_inode_seq;
}




//===========================================================================
// k_fwrite(struct s_v_inode * src_inode, s8 *data)
//===========================================================================
PUBLIC	u32	k_fwrite(s32 dev, struct s_v_inode * src_inode, s8 *data)
{
	u32	start_sec;
	s8 	*p = data;

	u32	block_size = vfs->spk.block_size;
	u32	inode_size = vfs->spk.inode_size;
	u32	len = strlen(data);

	//-----------------------------------------------------------------
	// init file[]
	//-----------------------------------------------------------------
	empty_buf(fsbuf, BLOCK_SIZE);
		
	//-----------------------------------------------------------------
	// delete entry
	//-----------------------------------------------------------------
	if(len <= block_size * 6)
	{
		struct s_mesg mesg;

		s8	s = len / block_size;

		u8 i;
		for(i=0; i<=s; i++)
		{
			switch(i)
			{
				case 0:
					start_sec = src_inode->i_block_0;
					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}					

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					p = p + block_size;
					break;
				case 1:
					if(!src_inode->i_block_1)
					{
						src_inode->i_block_1 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}

					start_sec = src_inode->i_block_1;

					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}					

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
					
					p = p + block_size;
					break;
				case 2:
					if(!src_inode->i_block_2)
					{
						src_inode->i_block_2 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}

					start_sec = src_inode->i_block_2;
					
					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
					
					p = p + block_size;
					break;
				case 3:
					if(!src_inode->i_block_3)
					{
						src_inode->i_block_3 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}

					start_sec = src_inode->i_block_3;
					
					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}					

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
					
					p = p + block_size;
					break;
					
				case 4:
					if(!src_inode->i_block_4)
					{
						src_inode->i_block_4 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}

					start_sec = src_inode->i_block_4;
					
					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);					
					
					p = p + block_size;
					break;
				case 5:
					if(!src_inode->i_block_5)
					{
						src_inode->i_block_5 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}

					start_sec = src_inode->i_block_5;
					
					if(i == s)
					{
						strcopy(fsbuf, p);						
					}
					else
					{
						strcpy(fsbuf, p, block_size);						
					}
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
					
					p = p + block_size;
					break;
			}
		}
	}
	else if(len <= block_size * 1024)
	{
		//for 1lvd
		start_sec = src_inode->i_block_1lvl_ind;

		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);
		
		u8 i;
		s8    s = len / block_size;
		s32 * addr = (s32 *)fsbuf;
						
		for(i=0; i<=s; i++, addr++)
		{
			empty_buf(tmp_fsbuf, BLOCK_SIZE);

			*addr = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;	// update the block numbers in fsbuf
			
			if(i == s)
			{
				strcopy(tmp_fsbuf, p);
			}
			else
			{
				strcpy(tmp_fsbuf, p, block_size);
			}
					
			struct s_mesg mesg;

			mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
			mesg.msg.parm1	= *addr;
			mesg.msg.parm2	= 1;			// read 1 sector
			mesg.src_pid	= PROC_FS;
			mesg.dest_pid	= PROC_HD;
			mesg.type	= HARDDISK_WRITE;
			mesg.func	= SYN;
		
			send_msg(&mesg);		

			p = p + block_size;			
		}

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_WRITE;
		mesg.func	= SYN;
		
		send_msg(&mesg);
	}
	else if(len <= block_size * 1024 * 1024)
	{
		//for 2lvd
	}
	
	//-----------------------------------------------------------------
	// update inode size
	//-----------------------------------------------------------------	
	src_inode->i_size = len / block_size;

	vfs->update_inode(dev, src_inode);	
}

//===========================================================================
// k_fappend(struct s_v_inode * src_inode, s8 *data)
//===========================================================================
PUBLIC	u32	k_fappend(s32 dev, struct s_v_inode * src_inode, s8 *data)
{
	u32	start_sec;
	s8 	*start;

	s8 	*p = data;

	u32	block_size 	= vfs->spk.block_size;
	u32	inode_size 	= vfs->spk.inode_size;
	
	u32	src_size   	= src_inode->i_size;
	s8	src_s	   	= src_size / block_size;
	
	u32	len 		= strlen(data);
	s8	dst_s 		= (src_size + len) / block_size;
	//-----------------------------------------------------------------
	// init file[]
	//-----------------------------------------------------------------
	empty_buf(fsbuf, BLOCK_SIZE);
	
	//-----------------------------------------------------------------
	// delete entry
	//-----------------------------------------------------------------
	if(dst_s < 6)
	{
		u8 i;
		struct s_mesg mesg;

		for(i=src_s; i<=dst_s; i++)
		{
			switch(i)
			{
				case 0:
					start_sec = src_inode->i_block_0;

					if(i == src_s)
					{
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);						

						start = fsbuf + src_size ;
					}
					else
					{
						start = fsbuf;						
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}				

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
				
					p = p + block_size;
					break;
				case 1:
					if(!src_inode->i_block_1)
					{
						src_inode->i_block_1 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}
					start_sec = src_inode->i_block_1;
					
					if(i == src_s)
					{
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);					

						start = fsbuf + src_size;
					}
					else
					{
						start = fsbuf;					
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);					
			
					p = p + block_size;
					break;
				case 2:
					if(!src_inode->i_block_2)
					{
						src_inode->i_block_2 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}
					start_sec = src_inode->i_block_2;
					
					if(i == src_s)
					{						
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);						

						start = fsbuf + src_size;
					}
					else
					{
						start = fsbuf;					
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}	

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);				
					
					p = p + block_size;
					break;
				case 3:
					if(!src_inode->i_block_3)
					{
						src_inode->i_block_3 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}
					start_sec = src_inode->i_block_3;
					
					if(i == src_s)
					{
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);					

						start = fsbuf + src_size;
					}
					else
					{						
						start = fsbuf;
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);					

					p = p + block_size;
					break;
					
				case 4:
					if(!src_inode->i_block_4)
					{
						src_inode->i_block_4 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}
					start_sec = src_inode->i_block_4;
					
					if(i == src_s)
					{
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);					

						start = fsbuf + src_size;
					}
					else
					{
						start = fsbuf;						
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}	
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					p = p + block_size;
					break;
				case 5:
					if(!src_inode->i_block_5)
					{
						src_inode->i_block_5 = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;
					}
					start_sec = src_inode->i_block_5;	
					
					if(i == src_s)
					{
						struct s_mesg mesg;

						mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
						mesg.msg.parm1	= start_sec;
						mesg.msg.parm2	= 1;			// read 1 sector
						mesg.src_pid	= PROC_FS;
						mesg.dest_pid	= PROC_HD;
						mesg.type	= HARDDISK_READ;
						mesg.func	= SYN;
		
						send_msg(&mesg);						

						start = fsbuf + src_size;
					}
					else
					{
						start = fsbuf;						
					}

					if(i == dst_s)
					{
						strcopy(start, p);
					}
					else
					{
						strcpy(start, p, (block_size - (start - fsbuf)));						
					}

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_WRITE;
					mesg.func	= SYN;
		
					send_msg(&mesg);
				
					p = p + block_size;
					break;
			}
		}
	}
	else if(dst_s <= 1024)
	{
		//for 1lvd
		start_sec = src_inode->i_block_1lvl_ind;

		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);		

		u8 i;
		s32 * addr = (s32 *)fsbuf;		
				
		for(i=src_s; i<=dst_s; i++, addr++)
		{
			empty_buf(tmp_fsbuf, BLOCK_SIZE);

			*addr = alloc_block(1) * SECTOR_PER_BLOCK / block_size ;	// update the block numbers in fsbuf
			
			if(i == src_s)
			{
				struct s_mesg mesg;

				mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
				mesg.msg.parm1	= start_sec;
				mesg.msg.parm2	= 1;			// read 1 sector
				mesg.src_pid	= PROC_FS;
				mesg.dest_pid	= PROC_HD;
				mesg.type	= HARDDISK_READ;
				mesg.func	= SYN;
		
				send_msg(&mesg);			

				start = tmp_fsbuf + src_size;
			}
			else
			{
				start = tmp_fsbuf;
			}

			if(i == dst_s)
			{
				strcopy(start, p);
			}
			else
			{				
				strcpy(start, p, (block_size - (start - tmp_fsbuf)));
			}
			
			struct s_mesg mesg;

			mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
			mesg.msg.parm1	= *addr;
			mesg.msg.parm2	= 1;			// read 1 sector
			mesg.src_pid	= PROC_FS;
			mesg.dest_pid	= PROC_HD;
			mesg.type	= HARDDISK_WRITE;
			mesg.func	= SYN;
		
			send_msg(&mesg);			

			p = p + block_size;			
		}		

		hd_write_blk(start_sec, (s16*)&fsbuf, 1);	// write fsbuf into hd		

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_WRITE;
		mesg.func	= SYN;
		
		send_msg(&mesg);		
	}
	else if(dst_s <= 1024 * 1024)
	{
		//for 2lvd
	}
	
	//-----------------------------------------------------------------
	// update inode size
	//-----------------------------------------------------------------
	src_inode->i_size = src_size + (len / block_size);

	vfs->update_inode(dev, src_inode);
}


//===========================================================================
// k_fread(struct s_v_inode * src_inode)
//===========================================================================
PUBLIC	u32	k_fread(s32 dev, struct s_v_inode * src_inode)
{
	u32	start_sec;
	s32	length;

	s8 	*p = fsbuf;	
	s8	*q = p_tmp_data;

	u32	block_size = vfs->spk.block_size;

	u32	len = src_inode->i_size * block_size;
	
	//-----------------------------------------------------------------
	// init file[]
	//-----------------------------------------------------------------
	empty_buf(fsbuf, BLOCK_SIZE);
	
	//-----------------------------------------------------------------
	// delete entry
	//-----------------------------------------------------------------
	if(len <= block_size * 6)
	{
		struct s_mesg mesg;

		s8	s = len / block_size;

		u8 i;
		for(i=0; i<=s; i++)
		{
			switch(i)
			{
				case 0:
					start_sec = src_inode->i_block_0;
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
				case 1:
					start_sec = src_inode->i_block_1;
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
				case 2:
					start_sec = src_inode->i_block_2;
					

					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
				case 3:
					start_sec = src_inode->i_block_3;
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
					
				case 4:
					start_sec = src_inode->i_block_4;
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
				case 5:
					start_sec = src_inode->i_block_5;
					
					mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
					mesg.msg.parm1	= start_sec;
					mesg.msg.parm2	= 1;			// read 1 sector
					mesg.src_pid	= PROC_FS;
					mesg.dest_pid	= PROC_HD;
					mesg.type	= HARDDISK_READ;
					mesg.func	= SYN;
		
					send_msg(&mesg);
					
					length	= BLOCK_SIZE;

					strcpy(q, fsbuf, length);
					q = q + length;

					break;
			}
		}
	}
	else if(len <= block_size * 1024)
	{
		//for 1lvd
		start_sec = src_inode->i_block_1lvl_ind;

		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);
		
		u8 i;
		s8    s = len / block_size;
		s32 * addr = fsbuf;
					
		for(i=0; i<=s; i++, addr++)
		{
			empty_buf(tmp_fsbuf, BLOCK_SIZE);
			
			struct s_mesg mesg;

			mesg.msg.buf1	= laddr_to_paddr(PROC_FS, tmp_fsbuf);
			mesg.msg.parm1	= start_sec;
			mesg.msg.parm2	= 1;			// read 1 sector
			mesg.src_pid	= PROC_FS;
			mesg.dest_pid	= PROC_HD;
			mesg.type	= HARDDISK_READ;
			mesg.func	= SYN;
		
			send_msg(&mesg);
			
			length	= BLOCK_SIZE;

			strcpy(q, tmp_fsbuf, length);
			q = q + length;
		
		}
		
	}
	else if(len <= block_size * 1024 * 1024)
	{
		//for 2lvd
	}
	
}


//===========================================================================
// k_ls(struct s_v_inode * src_inode)
//===========================================================================
PUBLIC	u32	k_ls(s32 dev, s8* file_name, struct s_v_inode * src_inode)
{
	//-----------------------------------------------------------------
	// init
	//-----------------------------------------------------------------
	struct s_v_dir_ent * v_dir_ent;
	struct s_v_inode *dst_inode;
	u32	inode_num;
	u32	blk, offset;

	s32	file_count;
	s8 	sattr[10];
			
	u32	inode_start_sec	= part->inode_start_sec;
	u32	nr_inode_per_block = vfs->spk.block_size / vfs->spk.inode_size;
	u32	inode_size	= vfs->spk.inode_size;

	empty_buf(fsbuf, BLOCK_SIZE);
	u32 start_sec	= src_inode->i_block_0;

	struct s_mesg mesg;

	mesg.msg.buf1	= fsbuf;
	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	//-----------------------------------------------------------------
	// ls 
	//-----------------------------------------------------------------
	printk("dir is %s\n", file_name);	

	printk("\nFile         Attr            Size\n");
	printk("-----------------------------------------\n");	

	for(v_dir_ent = fsbuf, file_count=0; v_dir_ent->dir_inode; v_dir_ent++, file_count++)
	{
		empty_buf(tmp_fsbuf, BLOCK_SIZE);

		inode_num	= v_dir_ent->dir_inode;

		blk		= (inode_num - 1) / nr_inode_per_block;
		offset		= (inode_num - 1) % nr_inode_per_block;
		
		start_sec	= inode_start_sec + blk * SECTOR_PER_BLOCK;		

		struct s_mesg mesg;		
		mesg.msg.buf1	= tmp_fsbuf;
		mesg.msg.parm1	= start_sec;
		mesg.msg.parm2	= 1;			// read 1 sector
		mesg.src_pid	= PROC_FS;
		mesg.dest_pid	= PROC_HD;
		mesg.type	= HARDDISK_READ;
		mesg.func	= SYN;
		
		send_msg(&mesg);		
		
		dst_inode = &tmp_fsbuf[offset * inode_size];		
		
		switch(dst_inode->i_mode)
		{
			case INODE_R_MODE:				
				sattr[0] = 'R';
				sattr[1] = NULL;
				break;
			case INODE_W_MODE:				
				sattr[0] = 'W';
				sattr[1] = NULL;
				break;
			case INODE_E_MODE:				
				sattr[0] = 'E';
				sattr[1] = NULL;
				break;
			case INODE_R_W_MODE:				
				sattr[0] = 'R';
				sattr[1] = 'W';
				sattr[2] = NULL;
				break;
			case INODE_R_W_E_MODE:				
				sattr[0] = 'R';
				sattr[1] = 'W';
				sattr[2] = 'E';
				sattr[3] = NULL;
				break;
			case INODE_CHAR_R_MODE:				
				sattr[0] = '(';
				sattr[1] = 'C';
				sattr[2] = 'H';
				sattr[3] = ')';
				sattr[4] = 'R';
				sattr[5] = NULL;
				break;
			case INODE_CHAR_W_MODE:				
				sattr[0] = '(';
				sattr[1] = 'C';
				sattr[2] = 'H';
				sattr[3] = ')';
				sattr[4] = 'W';
				sattr[5] = NULL;
				break;
			default:
				sattr[0] = 'N';
				sattr[1] = 'V';
				sattr[2] = NULL;
				break;
		}		
		printk("<%s>    %s",v_dir_ent->dir_name, sattr);
		printk("      %x", dst_inode->i_size);
		printk("\n");
	}
	
	printk("-----------------------------------------\n");	
	printk("total files : (%d)\n", file_count);
}



//===========================================================================
// k_ls(struct s_v_inode * src_inode)
//===========================================================================
PUBLIC	u32	k_cd(s32 dev, s8* file_name)
{
	struct s_mesg mesg;

	mesg.msg.buf1	= file_name;
	mesg.msg.parm1	= strlen(file_name);
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_SYS;
	mesg.type	= CHANGE_WORKDIR;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	
}

//===========================================================================
// k_ls(struct s_v_inode * src_inode)
//===========================================================================
PUBLIC	u32	k_pwd(s32 dev)
{

	struct s_mesg mesg;

	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_SYS;
	mesg.type	= PRINT_DIR;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}
