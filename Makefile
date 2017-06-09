#===========================================================================
# Makefile
#   Copyright (C) 2012 Free Software Foundation, Inc.
#   Originally by ZhaoFeng Liang <zhf.liang@hotmail.com>
#
#This file is part of DTHAS.
#
#DTHAS is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or 
#(at your option) any later version.
#
#DTHAS is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
#===========================================================================

ENTRYPOINT	= 0xC0000000

FD		= a.img
HD		= dthas.img

# Programs, flags, etc.
ASM		= nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
ASMIFLAGS	= -I inst/
ASMLFLAGS	= -I src/boot/
ASMLIBFLAGS	= -I src/include/ -I src/lib/ -f elf
ASMKFLAGS	= -I src/include/ -I src/kernel/ -f elf
CFLAGS		= -I src/include/ -I include/sys/ -c -fno-builtin -w -fno-stack-protector
LDFLAGS		= -Ttext $(ENTRYPOINT) -Map krnl.map
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYPOINT)
ARFLAGS		= rcs

# This Program
INSTALL		= inst/boot.bin inst/load.bin
BOOT		= src/boot/boot.bin src/boot/bloadkr.bin
KERNEL		= src/kernel/akrl.bin
LIB		= src/lib/adthas.a

OBJS		= src/kernel/krl.o src/kernel/8259a.o src/kernel/klib.o\
			src/kernel/proc.o src/kernel/clock.o src/kernel/signal.o\
			src/kernel/syscallc.o\
			src/mesg/mesg.o\
			src/keyboard/keyboard.o src/tty/tty.o\
			src/hd/hd.o src/fs/dts/fs.o src/fs/vfs.o src/hd/hd_manager.o\
			src/fs/vfslib.o src/hd/partition.o\
			src/sys/sys.o src/sys/time.o\
			src/uproc/uproc.o src/uproc/ucrt.o\
			src/uproc/mem_manage.o src/uproc/page.o\
			src/pci/pci.o src/net/net.o src/net/rtl8029.o\
			src/net/ethernet.o src/net/frame.o\
			src/net/icmp.o src/net/ipv4.o src/net/route.o\
			src/net/traceroutes.o src/net/udp.o\
			src/net/dy_routes.o src/net/igmp.o\
			src/net/tcp/tcp.o\
			src/net/ftp/ftp.o src/net/ftp/ftp_lib.o src/net/ftp/ftp_cmd.o\
			src/net/tcp/tcp_daemon.o\
			src/net/dhcp/dhcp.o src/net/dhcp/dhcp_lib.o\
			src/net/tftp/tftp.o src/net/tftp/tftp_lib.o\
			src/net/dns/dns.o src/net/dns/dns_lib.o\
			src/net/snmp/snmp.o src/net/snmp/snmp_lib.o\
			src/net/http/http.o src/net/http/http_lib.o src/net/http/http_cmd.o\
			src/net/smtp/smtp.o src/net/smtp/smtp_client.o src/net/smtp/smtp_cmd.o src/net/smtp/smtp_server.o\
			src/net/pop3/pop3.o src/net/pop3/pop3_client.o src/net/pop3/pop3_cmd.o src/net/pop3/pop3_server.o\
			src/net/telnet/telnet.o src/net/telnet/telnet_client.o\
			src/net/telnet/telnet_cmd.o src/net/telnet/telnet_server.o
			


	
LOBJS		=  src/lib/printf.o\
			src/lib/puts.o src/lib/string.o src/lib/libc.o\
			src/lib/mkdir.o	src/lib/rmdir.o src/lib/mkfile.o src/lib/rmfile.o\
			src/lib/cat.o src/lib/cat_a.o src/lib/ls.o src/lib/getmsg.o src/lib/sendmsg.o\
			src/lib/read.o src/lib/libsys.o src/lib/syscalla.o\
			src/lib/alloc.o src/lib/exit.o src/lib/enpage.o\
			src/lib/ulib.o src/lib/valloc.o src/lib/write.o\
			src/lib/readb.o src/lib/printk.o src/lib/cd.o\
			src/lib/pwd.o src/lib/send_package.o\
			src/lib/libn.o\
			src/lib/ping.o src/lib/netstat.o\
			src/lib/ifconfig.o\
			src/lib/gettimen.o\
			src/lib/traceroute.o src/lib/getroute.o\
			src/lib/igmpset.o\
			src/lib/ftpopen.o src/lib/ftpquit.o\
			src/lib/ftpuser.o src/lib/ftppasswd.o\
			src/lib/ftpls.o src/lib/ftpget.o\
			src/lib/ftpput.o src/lib/ftptype.o\
			src/lib/tcpsend.o\
			src/lib/dhcp_discover.o src/lib/dhcp_request.o src/lib/dhcp_release.o\
			src/lib/tftpread.o src/lib/tftpwrite.o\
			src/lib/dns_name2ip.o src/lib/dns_ip2name.o\
			src/lib/snmp_getrequest.o\
			src/lib/http_get.o src/lib/http_open.o\
			src/lib/smtp_send.o src/lib/smtp_open.o\
			src/lib/pop3_user.o src/lib/pop3_open.o\
			src/lib/telnet_open.o
			
			

			
DASMOUTPUT	= akrl.bin.asm

# All Phony Targets
.PHONY : everything final img clean realclean disasm all buildimg

# Default starting position

everything : $(INSTALL) $(BOOT) $(KERNEL) $(OBJS) $(LOBJS)

all : realclean everything

img : realclean everything clean buildimg

clean :
	rm -f $(OBJS) $(LOBJS)

realclean :
	rm -f $(INSTALL) $(BOOT) $(KERNEL) $(OBJS) $(LOBJS) $(LIB)

disasm :
	$(DASM) $(DASMFLAGS) $(KERNEL) > $(DASMOUTPUT)

buildimg :
	rm -f disc/dthas_setup.iso
	
	mkisofs -R -U -no-emul-boot -boot-load-size 4 -b inst/boot.bin -c inst/boot.cat -hide-rr-moved -x ./dthas.img -x .svn -x inst/.svn -x disc -o disc/dthas_setup.iso .

inst/boot.bin : inst/boot.asm
	$(ASM) -o $@ $<

inst/load.bin : inst/load.asm
	$(ASM) $(ASMIFLAGS) -o $@ $<

src/boot/boot.bin : src/boot/boot.asm
	$(ASM) -o $@ $<

src/boot/bloadkr.bin : src/boot/bloadkr.asm
	$(ASM) $(ASMLFLAGS) -o $@ $<

$(KERNEL) : $(OBJS) $(LIB)
	$(LD) $(LDFLAGS) -o $(KERNEL) $^

$(LIB) : $(LOBJS)
	$(AR) $(ARFLAGS) $@ $^

src/kernel/krl.o : src/kernel/krl.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

src/kernel/klib.o : src/kernel/klib.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

src/kernel/8259a.o: src/kernel/8259a.c
	$(CC) $(CFLAGS) -o $@ $<

src/kernel/proc.o: src/kernel/proc.c
	$(CC) $(CFLAGS) -o $@ $<

src/kernel/clock.o: src/kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<

src/kernel/signal.o: src/kernel/signal.c
	$(CC) $(CFLAGS) -o $@ $<

src/kernel/syscallc.o: src/kernel/syscallc.c
	$(CC) $(CFLAGS) -o $@ $<


src/mesg/mesg.o: src/mesg/mesg.c
	$(CC) $(CFLAGS) -o $@ $<

src/keyboard/keyboard.o: src/keyboard/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<

src/tty/tty.o: src/tty/tty.c
	$(CC) $(CFLAGS) -o $@ $<

src/hd/hd.o: src/hd/hd.c
	$(CC) $(CFLAGS) -o $@ $<

src/hd/hd_manager.o: src/hd/hd_manager.c
	$(CC) $(CFLAGS) -o $@ $<

src/hd/partition.o: src/hd/partition.c
	$(CC) $(CFLAGS) -o $@ $<

src/fs/dts/fs.o: src/fs/dts/fs.c
	$(CC) $(CFLAGS) -o $@ $<

src/fs/vfs.o: src/fs/vfs.c
	$(CC) $(CFLAGS) -o $@ $<

src/fs/vfslib.o: src/fs/vfslib.c
	$(CC) $(CFLAGS) -o $@ $<

src/sys/sys.o: src/sys/sys.c
	$(CC) $(CFLAGS) -o $@ $<

src/sys/time.o: src/sys/time.c
	$(CC) $(CFLAGS) -o $@ $<

src/uproc/uproc.o: src/uproc/uproc.c
	$(CC) $(CFLAGS) -o $@ $<

src/uproc/ucrt.o: src/uproc/ucrt.c
	$(CC) $(CFLAGS) -o $@ $<

src/uproc/mem_manage.o: src/uproc/mem_manage.c
	$(CC) $(CFLAGS) -o $@ $<

src/uproc/page.o: src/uproc/page.c
	$(CC) $(CFLAGS) -o $@ $<

src/pci/pci.o: src/pci/pci.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/ftp/ftp.o: src/net/ftp/ftp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/ftp/ftp_lib.o: src/net/ftp/ftp_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/ftp/ftp_cmd.o: src/net/ftp/ftp_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/net.o: src/net/net.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/frame.o: src/net/frame.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/rtl8029.o: src/net/rtl8029.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/ethernet.o: src/net/ethernet.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/icmp.o: src/net/icmp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/ipv4.o: src/net/ipv4.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/route.o: src/net/route.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/traceroutes.o: src/net/traceroutes.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/udp.o: src/net/udp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/dy_routes.o: src/net/dy_routes.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/igmp.o: src/net/igmp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/tcp/tcp.o: src/net/tcp/tcp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/tcp/tcp_daemon.o: src/net/tcp/tcp_daemon.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/tftp/tftp.o: src/net/tftp/tftp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/tftp/tftp_lib.o: src/net/tftp/tftp_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/dns/dns.o: src/net/dns/dns.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/dns/dns_lib.o: src/net/dns/dns_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/dhcp/dhcp.o: src/net/dhcp/dhcp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/dhcp/dhcp_lib.o: src/net/dhcp/dhcp_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/snmp/snmp.o: src/net/snmp/snmp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/snmp/snmp_lib.o: src/net/snmp/snmp_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/http/http.o: src/net/http/http.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/http/http_lib.o: src/net/http/http_lib.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/http/http_cmd.o: src/net/http/http_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/smtp/smtp.o: src/net/smtp/smtp.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/smtp/smtp_client.o: src/net/smtp/smtp_client.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/smtp/smtp_cmd.o: src/net/smtp/smtp_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/smtp/smtp_server.o: src/net/smtp/smtp_server.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/pop3/pop3.o: src/net/pop3/pop3.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/pop3/pop3_client.o: src/net/pop3/pop3_client.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/pop3/pop3_cmd.o: src/net/pop3/pop3_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/pop3/pop3_server.o: src/net/pop3/pop3_server.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/telnet/telnet.o: src/net/telnet/telnet.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/telnet/telnet_client.o: src/net/telnet/telnet_client.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/telnet/telnet_cmd.o: src/net/telnet/telnet_cmd.c
	$(CC) $(CFLAGS) -o $@ $<

src/net/telnet/telnet_server.o: src/net/telnet/telnet_server.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/printf.o: src/lib/printf.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/string.o: src/lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/libc.o: src/lib/libc.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/puts.o: src/lib/puts.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/mkdir.o: src/lib/mkdir.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/rmdir.o: src/lib/rmdir.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/mkfile.o: src/lib/mkfile.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/rmfile.o: src/lib/rmfile.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/cat.o: src/lib/cat.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/cat_a.o: src/lib/cat_a.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ls.o: src/lib/ls.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/getmsg.o: src/lib/getmsg.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/sendmsg.o: src/lib/sendmsg.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/read.o: src/lib/read.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/libsys.o: src/lib/libsys.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/syscalla.o : src/lib/syscalla.asm
	$(ASM) $(ASMLIBFLAGS) -o $@ $<

src/lib/alloc.o: src/lib/alloc.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/exit.o: src/lib/exit.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/enpage.o: src/lib/enpage.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ulib.o : src/lib/ulib.asm
	$(ASM) $(ASMLIBFLAGS) -o $@ $<

src/lib/valloc.o: src/lib/valloc.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/write.o: src/lib/write.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/readb.o: src/lib/readb.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/printk.o: src/lib/printk.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/cd.o: src/lib/cd.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/pwd.o: src/lib/pwd.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/send_package.o: src/lib/send_package.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ping.o: src/lib/ping.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/libn.o: src/lib/libn.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/netstat.o: src/lib/netstat.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ifconfig.o: src/lib/ifconfig.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/gettimen.o: src/lib/gettimen.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/traceroute.o: src/lib/traceroute.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/getroute.o: src/lib/getroute.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/igmpset.o: src/lib/igmpset.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpopen.o: src/lib/ftpopen.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpquit.o: src/lib/ftpquit.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpuser.o: src/lib/ftpuser.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftppasswd.o: src/lib/ftppasswd.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpls.o: src/lib/ftpls.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpget.o: src/lib/ftpget.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftpput.o: src/lib/ftpput.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/ftptype.o: src/lib/ftptype.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/tcpsend.o: src/lib/tcpsend.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/tftpread.o: src/lib/tftpread.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/tftpwrite.o: src/lib/tftpwrite.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/dns_ip2name.o: src/lib/dns_ip2name.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/dns_name2ip.o: src/lib/dns_name2ip.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/dhcp_discover.o: src/lib/dhcp_discover.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/dhcp_request.o: src/lib/dhcp_request.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/dhcp_release.o: src/lib/dhcp_release.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/snmp_getrequest.o: src/lib/snmp_getrequest.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/http_get.o: src/lib/http_get.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/http_open.o: src/lib/http_open.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/smtp_send.o: src/lib/smtp_send.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/smtp_open.o: src/lib/smtp_open.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/pop3_user.o: src/lib/pop3_user.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/pop3_open.o: src/lib/pop3_open.c
	$(CC) $(CFLAGS) -o $@ $<

src/lib/telnet_open.o: src/lib/telnet_open.c
	$(CC) $(CFLAGS) -o $@ $<
