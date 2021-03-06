//===========================================================================
// prototype.h
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

#ifndef	_X_PROTOTYPE_H_
#define	_X_PROTOTYPE_H_

//8259a.c
PUBLIC	void 	init_8259a();

//klib.asm
PUBLIC	void 	disp_num(u32 num);
PUBLIC	void 	disp_sym(char sym);
PUBLIC	void 	disp_dword(u32 value);
PUBLIC	u16  	in_16(u16 port);
PUBLIC	u8   	in_8(u16 port);
PUBLIC	void 	out_8(u16 port, u8 value);
PUBLIC	void 	out_16(u16 port, u16 value);
PUBLIC	void 	disp_str(char *str);
PUBLIC	void 	mcopy(u32 *dest, u32* src, u16 len);
PUBLIC	void 	en_irq(u32 irq);
PUBLIC	void 	dis_irq(u32 irq);
PUBLIC	void 	dis_int();
PUBLIC	void 	en_int();
PUBLIC 	void 	reboot();
PUBLIC 	void 	shutdown();
PUBLIC	u32	desc_baseaddr_get(u32 addr);
PUBLIC 	void 	phycopy(u8* dst, u8* src, u32 size);
PUBLIC 	s32 	gethddata(s16 * buf, u32 total_size);
PUBLIC 	s32 	puthddata(s16 * buf, u32 total_size);
PUBLIC	void	sys_enpage(u32 base_addr);
PUBLIC	void	page_set(u32 page_dir_base, u32 page_table_base, u32 new_proc_base);
PUBLIC	void	upd_gdt_descs_user(u32 index, u32 proc_base);
PUBLIC	u32  	in_32(u16 port);
PUBLIC	void 	out_32(u16 port, u32 value);

//keyboard.c
PUBLIC	void 	keyb_handler();
PUBLIC	void 	init_keyb();
PUBLIC	void 	char_exec(u8 scode);
PUBLIC	void 	reset_cmdlb();

//tty.c
PUBLIC	void 	init_tty();
PUBLIC	void 	tty_buf_write(u8 acode);
PUBLIC	void 	tty_read();
PUBLIC	void 	tty_write();
PUBLIC	void 	print_char(u8 schar);
PUBLIC	void 	tty_main();
PUBLIC	void 	reset_cursor(u32 addr);
PUBLIC	void 	scroll_down();
PUBLIC	void 	scroll_up();
PUBLIC	void 	tab_exec();
PUBLIC	void 	enter_exec();
PUBLIC	void 	backspace_exec();
PUBLIC	void 	delete_exec();
PUBLIC	void 	change_tty(u8 ttyn);
PUBLIC	void 	direction_exec(u8 direction);
PUBLIC	void 	home_exec();
PUBLIC	void 	end_exec();
PUBLIC 	int 	sys_puts(char *buf, int len);
PUBLIC 	int 	sys_prints(s8 *buf);
PUBLIC	void 	clear_screen();

//printf.c
PUBLIC	s32 	printf(char *cmd, ...);
PUBLIC 	s32 	printf_exec(char *cmd, char *value);

//write.c
PUBLIC	s32 write(s8 *buf, s32 start_sec, s32 nr_block);

//readb.c
PUBLIC	s32 readb(s16 *buf, s32 start_sec, s32 nr_block);

//lib.c
PUBLIC  void 	i2s(int val, char *buf);
PUBLIC	void 	i2h(int val, char *buf);
PUBLIC	void 	clear_buf(char *buf, int len);
PUBLIC 	void 	push(char *buf, char val);
PUBLIC	u8 	num2bit(u16 num);
PUBLIC	u16 	bit2num(u8 bit);
PUBLIC	void	delay(u32 num);
PUBLIC 	u8 	c2i(s8 ch);
PUBLIC 	u8 	s2i(s8 * str);
PUBLIC 	s32 	squ(s32 num, s32 ind);
PUBLIC 	u16 	big_little_16(u16 val);
PUBLIC 	u32 	big_little_32(u32 val);
PUBLIC 	u16 	little_big_16(u16 val);
PUBLIC 	u32 	little_big_32(u32 val);
PUBLIC	u8	bcd_2_i(u8 num);


//string.c
PUBLIC 	void 	strcopy(char *dest, char *src);
PUBLIC 	int 	strlen(char *buf);
PUBLIC 	void 	strcpy(char *dest, char *src, s32 len);
PUBLIC 	void 	memcpy(u8 *dest, u8 *src, s32 len);
PUBLIC 	s8 	strcmp(char *dest, char *src);

// hd.c
PUBLIC	void 	init_hd(void);
PUBLIC	void 	harddisk_driver(void);
PUBLIC	s32 	hd_read_blk (u32 start_sector, s16 * buf, u32 blk_cnt);
PUBLIC	s32 	hd_write_blk(u32 start_sector, s16 * buf, u32 blk_cnt);
PUBLIC	void 	get_hd_info(s16 *buf);
PUBLIC	void 	print_hd_info(struct s_hdinfo * hdinfo);
PUBLIC	void 	hd_main();

// partition.c
PUBLIC	void 	init_part();
PUBLIC	void 	print_part_info();

// dts/fs.c
PUBLIC	fs_op 	dts_get_fstab();
PUBLIC	void	dts_add_inode(s32 dev, s32 inode_seq_num);
PUBLIC	void	dts_add_block(s32 dev, s32 inode_seq_num, u32 size_in_blk);
PUBLIC	void	dts_add_imap_bit(s32 dev);
PUBLIC	void	dts_add_bmap_bit(s32 dev, s32 size_in_blk);
PUBLIC	s32	dts_add_dir_ent(s32 dev, u32 entry_start_sec, s8 * name);
PUBLIC	void	dts_del_inode(s32 dev, s32 inode_seq_num);
PUBLIC	void	dts_del_block(s32 dev, s32 inode_seq_num, u32 size_in_blk);
PUBLIC	void	dts_del_imap_bit(s32 dev);
PUBLIC	void	dts_del_bmap_bit(s32 dev, s32 size_in_blk);
PUBLIC	s32	dts_del_dir_ent(s32 dev, u32 entry_start_sec, s8 * name);
PUBLIC	void	dts_update_inode(s32 dev, struct s_dts_inode * src_inode);

// dts/fslib.c
PUBLIC	struct s_dts_super_blk * dts_get_super_blk(s32 dev);
PUBLIC	u32	dts_get_inode(s32 dev, u8 * ent_name, struct s_dts_inode * src_inode);
PUBLIC	u32 	dts_get_nr_inode_blks(s32 dev);
PUBLIC	u32 	dts_get_nr_imap_blks(s32 dev);
PUBLIC	u32 	dts_get_nr_blks(s32 dev);
PUBLIC	u32 	dts_get_nr_bmap_blks(s32 dev);

// vfs.c
PUBLIC	void 	init_vfs();
PUBLIC	void 	vfs_test();
PUBLIC	void 	vfs_main();
PUBLIC	s32 	sys_mkdir(s8 *dir_name);
PUBLIC	s32 	sys_mkfile(s8 *dir_name);
PUBLIC	s32 	sys_rmdir(s8 *dir_name);
PUBLIC	s32 	sys_rmfile(s8 *dir_name);
PUBLIC	s32 	sys_ls(s8 *file_name, s8 mode);
PUBLIC	s32 	sys_cat(s8 *file_name, s8 *data, s8 mode);
PUBLIC	s32 	sys_read(s8 *file_name, s8 mode);
PUBLIC	s32 	sys_cd(s8 *file_name);
PUBLIC	s32 	sys_pwd(void);

// hd_manager.c
PUBLIC	void	init_hd_freelist();
PUBLIC	void	printflist();
PUBLIC	u32	alloc_block(u32 nr_block);
PUBLIC	u32	free_block(u32 addr, u32 nr_block);

// mkdir.c
PUBLIC	s32 	mkdir(s8 *dir_name);

// rmdir.c
PUBLIC	s32 	rmdir(s8 *dir_name);

// mkfile.c
PUBLIC	s32 	mkfile(s8 *dir_name);

// rmfile.c
PUBLIC	s32 	rmfile(s8 *dir_name);

// cat.c
PUBLIC	s32 	cat(s8 *dir_name, s8 *data, s8 mode);

// cat_a.c
PUBLIC	s32 	cat_a(s8 *file_name, s8 *data, s8 mode);

// ls.c
PUBLIC	s32 	ls(s8 *file_name, s8 mode);

// cd.c
PUBLIC	s32 	cd(s8 *file_name);

// pwd.c
PUBLIC	s32 	pwd(void);

// ping.c 
PUBLIC	s32 	ping(s8 *buf);

// netstat.c
PUBLIC	s32 	netstat();

// ifconfig.c 
PUBLIC	s32 	ifconfig(s8 *buf);

// gettimen.c 
PUBLIC	s32 	gettimen();

// traceroute.c 
PUBLIC	s32 traceroute(s8 *buf);


// alloc.c
PUBLIC	s32 	alloc(s32 size);

// vfslib.c
PUBLIC	void 	k_mkinode(u32 entry_start_sec, char *dir_name);
PUBLIC	void 	k_rminode(u32 entry_start_sec, char *dir_name);
PUBLIC	u32 	k_fopen(s32 dev, u8 * file_name, struct s_dts_inode * src_inode);
PUBLIC	void	empty_buf(u32 start_addr, u32 size_in_byte);
PUBLIC	u32	k_fwrite(s32 dev, struct s_v_inode * src_inode, s8 *data);
PUBLIC	u32	k_fread(s32 dev, struct s_v_inode * src_inode);
PUBLIC	u32	k_fappend(s32 dev, struct s_v_inode * src_inode, s8 *data);
PUBLIC	u32	k_ls(s32 dev, s8* file_name, struct s_v_inode * src_inode);
PUBLIC	u32	k_cd(s32 dev, s8* file_name);
PUBLIC	u32	k_pwd(s32 dev);


// proc.c
PUBLIC	void	init_sys_proc(proc_t proc_addr, struct proc *p, u8 * stack, s32 seq_num);
PUBLIC	void	init_proc();
PUBLIC	void	init_user_proc(proc_t proc_addr, struct proc *p, u8 * stack, s32 seq_num);
PUBLIC	void	init_default_proc(struct proc *p, s32 seq_num);
PUBLIC 	void	init_desc(struct p_desc * desc, u32 base, u32 limit, u16 attr);
PUBLIC	void 	test1();
PUBLIC	void 	test2();
PUBLIC	void 	test3();
PUBLIC	u32	paddr_to_laddr(u32 pid, u32 paddr);
PUBLIC	u32	laddr_to_paddr(u32 pid, u32 laddr);


// clock.c
PUBLIC	void 	init_clock();
PUBLIC 	void	clock_driver();

// signal.c
PUBLIC	void 	sig_process(s32 pid);
PUBLIC	void 	en_sig(s32 pid, u32 sig);
PUBLIC	void 	dis_sig(s32 pid, u32 sig);
PUBLIC	void 	clear_sig(s32 pid, u32 sig);
PUBLIC	void 	set_sig(s32 pid, u32 sig);
PUBLIC	void 	sigkill(s32 pid, u32 sig);
PUBLIC	u8 	sig_mask_check(s32 pid, u32 sig);

// syscallc.c


// mesg.c
PUBLIC	void	init_mesg();
PUBLIC	void	mesg_main();
PUBLIC	void	out_queue(struct s_mesg *msg);
PUBLIC	void	in_queue(struct s_mesg *msg);
PUBLIC 	int 	sys_send(struct s_mesg *msg);
//PUBLIC	void	sys_forward(struct s_mesg *msg);
PUBLIC	s32	sys_forward();
PUBLIC	s32	sys_receive();

// getmsg.c
PUBLIC 	s32 	get_msg(struct s_mesg *mesg);

// sendmsg.c
PUBLIC s32 send_msg(struct s_mesg *mesg);

// syscalla.asm
PUBLIC s32 send(struct s_mesg *mesg);

// sys.c
PUBLIC	void	sys_main();
PUBLIC	void	sys_status(struct s_mesg *msg);
PUBLIC	void	sleep(s32 pid);
PUBLIC	void	wakeup(s32 pid);
PUBLIC	void	sys_attr(struct s_mesg *msg);
PUBLIC	void	change_dir(s8 *dir, s32 len);

// time.c
PUBLIC	void	systime_init();
PUBLIC	void	systime_set();
PUBLIC	void	delay_sec(u32 sec);

// enpage.c
PUBLIC int enpage(u32 base_addr);

// valloc.c
PUBLIC	s32 	valloc(s32 nr);

// uproc.c
PUBLIC	void 	cmd_exec(s8 *buf);
PUBLIC	void 	uproc_main();
PUBLIC	void 	init_uproc();
PUBLIC	void 	stack_setup(u32 stack_base_p, u32 stack_base_v, s32 argc, u32 argv_addr);
PUBLIC	struct proc *	sys_fork();
PUBLIC	s32	sys_exit(s32 pid);
PUBLIC	s32	sys_execl(struct proc *new_p, s8 *filename, s32 argc, u32 argv_addr);
PUBLIC	void	user_page_set(u32 base_addr);

// mem_manage.c
PUBLIC	s32	sys_alloc(s32 size_in_bytes);
PUBLIC	s32	sys_free(u32 addr, s32 nr_bytes);
PUBLIC	void	init_mem_freelist();
PUBLIC	void	print_memflist();
PUBLIC	s32	sys_valloc(s32 nr);


// libsys.c
PUBLIC	u32	l2p_addr(u32 pid, u32 laddr);
PUBLIC	s32	cpid();

// ulib.asm
PUBLIC u32 	u_desc_baseaddr_get(u32 addr, u32 gdt_desc);

//page.c
PUBLIC	u32	page_driver(u32 page_laddr);
PUBLIC	s32	swapin(u32 page_laddr);
PUBLIC	s32	swapout(struct s_vmem_map * vmap);

//puts.c
PUBLIC 	s32 	puts(s8 *buf, s32 len);

//printk.c
PUBLIC	s32 	printk(char *cmd, ...);
PUBLIC 	s32 	prints(s8 *buf);

//libn.c
PUBLIC 	u16 	makechksum(u8 *pkg, u16 num);
PUBLIC 	void	s2ip(struct iaddr * ip, s8 *buf);
PUBLIC 	s8 	chk_src_dest_mac(struct hwaddr src_mac, struct hwaddr dest_mac);
PUBLIC 	s8 	pr_mac(struct hwaddr mac);

//net.c
PUBLIC	void 	init_net();
PUBLIC	void 	net_main();
PUBLIC	void 	pkg_process(struct s_package *pkg);

//ftpopen.c
PUBLIC	s32 	ftpopen(s8 *buf);

//ftpquit.c
PUBLIC	s32 	ftpquit(s8 *buf);

//ftpuser.c
PUBLIC	s32 	ftpuser(s8 *ip);

//ftppasswd.c
PUBLIC	s32 	ftppasswd(s8 *ip);

//ftpls
PUBLIC	s32 	ftpls(s8 *buf);

//ftpget
PUBLIC	s32 	ftpget(s8 *buf);

//ftpput
PUBLIC	s32 	ftpput(s8 *buf);

//ftptype
PUBLIC	s32 	ftptype(s8 *buf);

//tcpsend
PUBLIC	s32 	tcpsend(s8 *buf);

//tftpread
PUBLIC	s32 	tftpread(s8 *buf);

//tftpwrite
PUBLIC	s32 	tftpwrite(s8 *buf);

//dns_ip2name
PUBLIC	s32 	dns_name2ip(s8 *buf);

//dns_name2ip
PUBLIC	s32 	dns_ip2name(s8 *buf);

//snmp_getrequest
PUBLIC	s32 	snmp_getrequest(s8 *buf);

//http_get
PUBLIC	s32 	http_get(s8 *buf);

//httpopen
PUBLIC	s32 	httpopen(s8 *buf);

//smtp_send
PUBLIC	s32 	smtp_send(s8 *buf);

//smtp_open
PUBLIC	s32 	smtp_open(s8 *buf);

//pop3_user
PUBLIC	s32 	pop3_user(s8 *buf);

//pop3_open
PUBLIC	s32 	pop3_open(s8 *buf);

//telnet_open
PUBLIC	s32 	telnet_open(s8 *buf);



//pci.c
PUBLIC	void 	init_pci();
PUBLIC	void 	pci_scan(struct pci_device *pdev);
PUBLIC	void 	pci_disp();

//rtl8029.c
PUBLIC	void 	init_rtl8029();
PUBLIC	void 	rtl8029_handler();
PUBLIC	void 	rtl8029_send_pkg(struct s_package * pkg);
PUBLIC	s8 	rtl8029_receive_pkg(struct s_package * pkg);
PUBLIC	void 	rtl8029_main();
PUBLIC	void 	init_rtl8029_g();

//ethernet.c
PUBLIC 	void 	pr_ethernet();


//frame.c
PUBLIC	void 	frame_process(struct s_package *pkg);
PUBLIC	void 	build_frame(struct s_package *pkg, u16 type, u16 op, struct hwaddr dst_mac, struct iaddr dst_ip, struct hwaddr src_mac, struct iaddr src_ip);
PUBLIC	void 	add_frame_header(struct s_package *pkg, u16 type, struct hwaddr dst_mac, struct hwaddr src_mac);

//ipv4.c
PUBLIC	void 	add_ipv4_header(struct s_package *pkg, struct iaddr src_ip, u8 ttl, struct iaddr dst_ip, u8 protocol, u8 tos, u16 total_len, u16 offset, u8 flag);
PUBLIC	void 	ip_forward_v4(struct s_package *pkg);
PUBLIC	void 	ipv4_process(struct s_package *pkg, struct iaddr src_ip, struct iaddr dst_ip, u8 protocol, u16 data_len);
PUBLIC	void 	ipv4_send(struct s_package *pkg, struct iaddr src_ip, u8 ttl, struct iaddr dst_ip, u8 protocol, u8 tos, u16 ip_len, u16 offset, u8 flag);
PUBLIC	void 	ip_process(struct s_package *pkg, struct iaddr src_ip, struct iaddr dst_ip, u8 protocol, u16 data_length);

//icmp.c
PUBLIC	void 	icmp_process(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code, u16 iden, u16 seq);
PUBLIC	void 	icmp_echoreply(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code, u16 iden, u16 seq);

//route.c
PUBLIC	void	init_routetbl();
PUBLIC	void	add_route(struct route_table *rtbl);
PUBLIC	void	del_route(struct route_table *rtbl);
PUBLIC	void	get_routemac(struct route_table *rtbl);
PUBLIC struct route_table *search_route(struct iaddr src_ip);
PUBLIC 	void 	update_routetable(struct iaddr gateway_ip, struct hwaddr gmac);
PUBLIC	void	pr_routetbl();
PUBLIC	u8	is_same_net(struct iaddr src_ip, struct iaddr src_mask, struct iaddr dst_ip, struct iaddr dst_mask);
PUBLIC struct iaddr get_default_gw();

//traceroutes.c
PUBLIC	void 	sys_traceroute(struct s_package *pkg, struct iaddr dst_ip);

//dy_routes.c
PUBLIC	void 	dyroutes_process(struct s_package *pkg, u8 type, u8 code);
PUBLIC	void 	rip1_process(struct s_package *pkg, u8 code);
PUBLIC	void 	rip2_process(struct s_package *pkg, u8 code);
PUBLIC	void 	ospf2_process(struct s_package *pkg, u8 code);
PUBLIC	void 	bgp_process(struct s_package *pkg, u8 code);

//igmp
PUBLIC	s32 	igmpset();
PUBLIC	void 	igmp_process(struct s_package *pkg, u8 type, u8 code);
PUBLIC	void 	igmp3_process(struct s_package *pkg, u8 code);

//udp.c
PUBLIC	void 	add_udp_header(struct s_package *pkg, struct iaddr src_ip, u16 udp_len, struct iaddr dst_ip, u8 protocol,u16 src_port, u16 dst_port);

//tcp.c
PUBLIC	void 	add_tcp_header(struct s_package *pkg, struct iaddr src_ip, u16 tcp_len, struct iaddr dst_ip, u8 protocol,u16 src_port, u16 dst_port, u32 seq, u32 ack, u8 header_len, u8 ctrl_bit, u16 winsize, u16 upointer);
PUBLIC	void	init_tcp();
PUBLIC	void 	tcp_process(struct s_package *src_pkg, struct s_package *dst_pkg, u16 src_tcp_data_len, u16 dst_tcp_data_len, u8 tos);
PUBLIC	u32	get_seq(void);

//ftp.c
PUBLIC	void 	init_ftp();
PUBLIC	void 	ftp_main();
PUBLIC	void 	ftp_ctrl_process(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	void 	ftp_data_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//ftp_lib.c
PUBLIC void	ftp_open(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC void	ftp_quit(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC void	ftp_user(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC void	ftp_passwd(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC u32	ftp_get(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC u32	ftp_ls(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC void	ftp_parm_save();
PUBLIC void	ftp_parm_init();
PUBLIC void	ftp_parm_restore();
PUBLIC u32	ftp_put(struct s_package *dst_pkg, struct iaddr dst_ip);
PUBLIC u32	ftp_type(struct s_package *dst_pkg, struct iaddr dst_ip);


//ftp_cmd.c
PUBLIC s32	ftp_cmd_user_pre(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_user(struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_pass(struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_syst(struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_quit(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_port(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC s32	ftp_cmd_put(struct s_package *src_pkg, struct s_package *dst_pkg);

//tcp_daemon
PUBLIC	void	tcp_daemon_main();
PUBLIC	void	init_tcp_daemon();

//tftp.c
PUBLIC	void	init_tftp();
PUBLIC	void 	tftp_main();
PUBLIC	void 	tftp_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//tftp_lib.c
PUBLIC	u32	get_port(void);

//dns.c
PUBLIC	void 	dns_main();
PUBLIC	void	init_dns();
PUBLIC	void 	dns_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//dns_lib.c
PUBLIC	void 	add_dns_header(struct s_package *pkg, u16 id, u16 flag, u16 question_cnt, u16 answer_cnt, u16 auth_cnt, u16 addiction_cnt);
PUBLIC	s32 	dns_send_ip(struct iaddr dst_ip, struct s_package *dst_pkg, struct iaddr res_ip);
PUBLIC	s32 	dns_send_name(struct iaddr dst_ip, struct s_package *dst_pkg, s8* domain_name_format);
PUBLIC	s32 	dns_return_name(struct s_package *src_pkg, struct s_package *dst_pkg, s8* domain_name_format);
PUBLIC	s32 	dns_return_ip(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip);
PUBLIC	u16	get_id(void);

//dhcp.c
PUBLIC	void	init_dhcp();
PUBLIC	void 	dhcp_main();
PUBLIC	void 	dhcp_process(struct s_package *src_pkg, struct s_package *dst_pkg);


//dhcp_lib.c
PUBLIC	void 	add_dhcp_header(struct s_package *pkg, u8 opcode, u8 hwtype, u8 hwlen, u8 ttl, u32 tran_id, u16 sec, u16 flag, struct iaddr client_ip, struct iaddr my_ip,	struct iaddr server_ip,	struct iaddr gw_ip, struct hwaddr client_mac, s8 *option);
PUBLIC	u32	get_transid(void);
PUBLIC	s32 	dhcp_send_nack(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip);
PUBLIC	s32 	dhcp_send_ack(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip);
PUBLIC	s32 	dhcp_send_offer(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip);
PUBLIC	s32 	dhcp_send_release(struct iaddr dst_ip, struct s_package *dst_pkg);
PUBLIC	s32 	dhcp_send_request(struct iaddr dst_ip, struct s_package *dst_pkg, struct iaddr req_ip);
PUBLIC	s32 	dhcp_send_discover(struct iaddr dst_ip, struct s_package *dst_pkg);

//snmp.c
PUBLIC	void 	snmp_main();
PUBLIC	void	init_snmp();
PUBLIC	void 	snmp_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//snmp_lib.c
PUBLIC	s32 	getrequest(struct iaddr dst_ip, struct s_package *dst_pkg);

//http.c
PUBLIC	void 	http_main();
PUBLIC	void	init_http();
PUBLIC	void 	http_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//http_lib.c
PUBLIC	void 	parm_init();
PUBLIC	s32 	http_req_get(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

//http_cmd.c
PUBLIC void	http_open(struct iaddr dst_ip, struct s_package *dst_pkg);
PUBLIC void	http_close(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	http_res_get(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

//smtp.c
PUBLIC	void 	smtp_main();
PUBLIC	void	init_smtp();
PUBLIC	void 	smtp_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//smtp_client.c
PUBLIC	void 	smtp_parm_init();
PUBLIC	s32 	smtp_send_sys(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

PUBLIC	s32 	smtp_send_ehlo(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_mailfrom(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_rcptto(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_data(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_mail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

//smtp_server.c
PUBLIC	s32 	smtp_send_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_ready(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_startmail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	smtp_send_close(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

//smtp_cmd.c
PUBLIC void	smtp_open_sys(struct iaddr dst_ip, struct s_package *dst_pkg);
PUBLIC void	smtp_close_sys(struct s_package *src_pkg, struct s_package *dst_pkg);

//pop3.c
PUBLIC	void 	pop3_main();
PUBLIC	void	init_pop3();
PUBLIC	void 	pop3_process(struct s_package *src_pkg, struct s_package *dst_pkg);

//pop3_client.c
PUBLIC	void 	pop3_parm_init();
PUBLIC	s32 	pop3_send_user(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_pass(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_mailinform(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_maillist(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_retr(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);


//pop3_server.c
PUBLIC	s32 	pop3_send_ready(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_user_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_pass_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_mailinform_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_maillist_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_mail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	pop3_send_quit_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);


//pop3_cmd.c
PUBLIC void	pop3_open_sys(struct iaddr dst_ip, struct s_package *dst_pkg);
PUBLIC void	pop3_close_sys(struct s_package *src_pkg, struct s_package *dst_pkg);

//telnet.c
PUBLIC	void 	telnet_process(struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	void	init_telnet();
PUBLIC	void 	telnet_main();

//telnet_server.c
PUBLIC	s32 	telnet_send_usertitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_passtitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_res_ls(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_res_ctrl(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_logintitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_res_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);

//telnet_cmd.c
PUBLIC void	telnet_open_sys(struct iaddr dst_ip, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_quit_sys(struct iaddr dst_ip,  struct s_package *dst_pkg);

//telnet_client.c
PUBLIC	s32 	telnet_send_username(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_password(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_ls(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_ctrl(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);
PUBLIC	s32 	telnet_send_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg);


#endif
