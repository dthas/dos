#===========================================================================
# Makefile
#   Copyright (C) 2012 Free Software Foundation, Inc.
#   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
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

HD		= ../../../dthas.img
ASM		= nasm
DASM		= objdump
CC		= gcc
LD		= ld
ASMFLAGS	= -I ../../include/ -f elf
CFLAGS		= -I ../../include/ -c -fno-builtin -fno-stack-protector -w
LDFLAGS		= -Ttext 0x600000 
DASMFLAGS	= -D
LIB		= ../../lib/adthas.a
BIN_SYS		= ../sys/echo ../sys/mkdir ../sys/mkfile ../sys/cat ../sys/ls ../sys/rmfile\
			 ../sys/rmdir ../sys/cd ../sys/pwd

BIN_NET		= ../net/ping ../net/netstat ../net/ifconfig\
			../net/gettimen ../net/traceroute ../net/getroute\
			../net/igmpset

BIN_FTP		= ../ftp/ftpopen ../ftp/ftpquit ../ftp/ftpuser\
			../ftp/ftppasswd ../ftp/ftpls ../ftp/ftpget ../ftp/ftpput ../ftp/ftptype

BIN_TFTP	= ../tftp/tftpread ../tftp/tftpwrite

BIN_DNS		= ../dns/dnsip2name ../dns/dnsname2ip

BIN_DHCP	= ../dhcp/dhcpdiscover ../dhcp/dhcprequest ../dhcp/dhcprelease

BIN_SNMP	= ../snmp/snmpgetrequest

BIN_HTTP	= ../http/httpget ../http/httpopen

BIN_SMTP	= ../smtp/smtpsend ../smtp/smtpopen

BIN_POP3	= ../pop3/pop3user ../pop3/pop3open

BIN_TELNET	= ../telnet/telnetopen

OBJS		=  echo/start.o echo/u_echo.o\
			mkdir/start.o mkdir/u_mkdir.o\
			mkfile/start.o mkfile/u_mkfile.o\
			cat/start.o cat/u_cat.o\
			ls/start.o ls/u_ls.o\
			rmfile/start.o rmfile/u_rmfile.o\
			rmdir/start.o rmdir/u_rmdir.o\
			cd/start.o cd/u_cd.o\
			pwd/start.o pwd/u_pwd.o\
			ping/start.o ping/u_ping.o\
			netstat/start.o netstat/u_netstat.o\
			ifconfig/start.o ifconfig/u_ifconfig.o\
			gettimen/start.o gettimen/u_gettimen.o\
			traceroute/start.o traceroute/u_traceroute.o\
			getroute/start.o getroute/u_getroute.o\
			igmpset/start.o igmpset/u_igmpset.o\
			ftpopen/start.o ftpopen/u_ftpopen.o\
			ftpquit/start.o ftpquit/u_ftpquit.o\
			ftpuser/start.o ftpuser/u_ftpuser.o\
			ftppasswd/start.o ftppasswd/u_ftppasswd.o\
			ftpls/start.o ftpls/u_ftpls.o\
			ftpget/start.o ftpget/u_ftpget.o\
			ftpput/start.o ftpput/u_ftpput.o\
			ftptype/start.o ftptype/u_ftptype.o\
			tftpread/start.o tftpread/u_tftpread.o\
			tftpwrite/start.o tftpwrite/u_tftpwrite.o\
			dnsname2ip/start.o dnsname2ip/u_dnsname2ip.o\
			dnsip2name/start.o dnsip2name/u_dnsip2name.o\
			dhcpdiscover/start.o dhcpdiscover/u_dhcpdiscover.o\
			dhcprequest/start.o dhcprequest/u_dhcprequest.o\
			dhcprelease/start.o dhcprelease/u_dhcprelease.o\
			snmpgetrequest/start.o snmpgetrequest/u_snmpgetrequest.o\
			httpget/start.o	httpget/u_httpget.o\
			httpopen/start.o httpopen/u_httpopen.o\
			smtpsend/start.o smtpsend/u_smtpsend.o\
			smtpopen/start.o smtpopen/u_smtpopen.o\
			pop3user/start.o pop3user/u_pop3user.o\
			pop3open/start.o pop3open/u_pop3open.o\
			telnetopen/start.o telnetopen/u_telnetopen.o
			

# All Phony Targets
.PHONY : everything final clean realclean disasm all install

# Default starting position
everything : $(BIN_SYS) $(BIN_NET) $(BIN_FTP) $(BIN_TFTP) $(BIN_DNS) $(BIN_DHCP) $(BIN_SNMP) $(BIN_HTTP) $(BIN_SMTP) $(BIN_POP3) $(BIN_TELNET)


install : all clean
	
all : realclean everything clean

final : all clean

clean :
	rm -f $(OBJS)

realclean :
	rm -f $(BIN) $(OBJS)

echo/start.o : echo/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

echo/u_echo.o: echo/u_echo.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/echo : echo/start.o echo/u_echo.o $(LIB)
	$(LD) $(LDFLAGS) -Map echo.map -o $@ $?

mkdir/start.o : mkdir/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

mkdir/u_mkdir.o: mkdir/u_mkdir.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/mkdir : mkdir/start.o mkdir/u_mkdir.o $(LIB)
	$(LD) $(LDFLAGS)  -Map mkdir.map -o $@ $?

mkfile/start.o : mkfile/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

mkfile/u_mkfile.o: mkfile/u_mkfile.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/mkfile : mkfile/start.o mkfile/u_mkfile.o $(LIB)
	$(LD) $(LDFLAGS)  -Map mkfile.map -o $@ $?

cat/start.o : cat/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

cat/u_cat.o: cat/u_cat.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/cat : cat/start.o cat/u_cat.o $(LIB)
	$(LD) $(LDFLAGS)  -Map cat.map -o $@ $?

ls/start.o : ls/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ls/u_ls.o: ls/u_ls.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/ls : ls/start.o ls/u_ls.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ls.map -o $@ $?

rmfile/start.o : rmfile/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

rmfile/u_rmfile.o: rmfile/u_rmfile.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/rmfile : rmfile/start.o rmfile/u_rmfile.o $(LIB)
	$(LD) $(LDFLAGS)  -Map rmfile.map -o $@ $?

rmdir/start.o : rmdir/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

rmdir/u_rmdir.o: rmdir/u_rmdir.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/rmdir : rmdir/start.o rmdir/u_rmdir.o $(LIB)
	$(LD) $(LDFLAGS)  -Map rmdir.map -o $@ $?

cd/start.o : cd/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

cd/u_cd.o: cd/u_cd.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/cd : cd/start.o cd/u_cd.o $(LIB)
	$(LD) $(LDFLAGS)  -Map cd.map -o $@ $?

pwd/start.o : pwd/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

pwd/u_pwd.o: pwd/u_pwd.c
	$(CC) $(CFLAGS) -o $@ $<

../sys/pwd : pwd/start.o pwd/u_pwd.o $(LIB)
	$(LD) $(LDFLAGS)  -Map pwd.map -o $@ $?

ping/start.o : ping/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ping/u_ping.o: ping/u_ping.c
	$(CC) $(CFLAGS) -o $@ $<

../net/ping : ping/start.o ping/u_ping.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ping.map -o $@ $?

netstat/start.o : netstat/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

netstat/u_netstat.o: netstat/u_netstat.c
	$(CC) $(CFLAGS) -o $@ $<

../net/netstat : netstat/start.o netstat/u_netstat.o $(LIB)
	$(LD) $(LDFLAGS)  -Map netstat.map -o $@ $?

ifconfig/start.o : ifconfig/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ifconfig/u_ifconfig.o: ifconfig/u_ifconfig.c
	$(CC) $(CFLAGS) -o $@ $<

../net/ifconfig : ifconfig/start.o ifconfig/u_ifconfig.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ifconfig.map -o $@ $?

gettimen/start.o : gettimen/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

gettimen/u_gettimen.o: gettimen/u_gettimen.c
	$(CC) $(CFLAGS) -o $@ $<

../net/gettimen : gettimen/start.o gettimen/u_gettimen.o $(LIB)
	$(LD) $(LDFLAGS)  -Map gettimen.map -o $@ $?

traceroute/start.o : traceroute/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

traceroute/u_traceroute.o: traceroute/u_traceroute.c
	$(CC) $(CFLAGS) -o $@ $<

../net/traceroute : traceroute/start.o traceroute/u_traceroute.o $(LIB)
	$(LD) $(LDFLAGS)  -Map traceroute.map -o $@ $?

getroute/start.o : getroute/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

getroute/u_getroute.o: getroute/u_getroute.c
	$(CC) $(CFLAGS) -o $@ $<

../net/getroute : getroute/start.o getroute/u_getroute.o $(LIB)
	$(LD) $(LDFLAGS)  -Map getroute.map -o $@ $?

igmpset/start.o : igmpset/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

igmpset/u_igmpset.o: igmpset/u_igmpset.c
	$(CC) $(CFLAGS) -o $@ $<

../net/igmpset : igmpset/start.o igmpset/u_igmpset.o $(LIB)
	$(LD) $(LDFLAGS)  -Map igmpset.map -o $@ $?

ftpopen/start.o : ftpopen/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpopen/u_ftpopen.o: ftpopen/u_ftpopen.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpopen : ftpopen/start.o ftpopen/u_ftpopen.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpopen.map -o $@ $?

ftpquit/start.o : ftpquit/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpquit/u_ftpquit.o: ftpquit/u_ftpquit.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpquit : ftpquit/start.o ftpquit/u_ftpquit.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpquit.map -o $@ $?

ftpuser/start.o : ftpuser/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpuser/u_ftpuser.o: ftpuser/u_ftpuser.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpuser : ftpuser/start.o ftpuser/u_ftpuser.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpuser.map -o $@ $?

ftppasswd/start.o : ftppasswd/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftppasswd/u_ftppasswd.o: ftppasswd/u_ftppasswd.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftppasswd : ftppasswd/start.o ftppasswd/u_ftppasswd.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftppasswd.map -o $@ $?

ftpls/start.o : ftpls/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpls/u_ftpls.o: ftpls/u_ftpls.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpls : ftpls/start.o ftpls/u_ftpls.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpls.map -o $@ $?

ftpget/start.o : ftpget/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpget/u_ftpget.o: ftpget/u_ftpget.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpget : ftpget/start.o ftpget/u_ftpget.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpget.map -o $@ $?

ftpput/start.o : ftpput/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftpput/u_ftpput.o: ftpput/u_ftpput.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftpput : ftpput/start.o ftpput/u_ftpput.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftpput.map -o $@ $?

ftptype/start.o : ftptype/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

ftptype/u_ftptype.o: ftptype/u_ftptype.c
	$(CC) $(CFLAGS) -o $@ $<

../ftp/ftptype : ftptype/start.o ftptype/u_ftptype.o $(LIB)
	$(LD) $(LDFLAGS)  -Map ftptype.map -o $@ $?

tftpread/start.o : tftpread/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

tftpread/u_tftpread.o: tftpread/u_tftpread.c
	$(CC) $(CFLAGS) -o $@ $<

../tftp/tftpread : tftpread/start.o tftpread/u_tftpread.o $(LIB)
	$(LD) $(LDFLAGS)  -Map tftpread.map -o $@ $?

tftpwrite/start.o : tftpwrite/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

tftpwrite/u_tftpwrite.o: tftpwrite/u_tftpwrite.c
	$(CC) $(CFLAGS) -o $@ $<

../tftp/tftpwrite : tftpwrite/start.o tftpwrite/u_tftpwrite.o $(LIB)
	$(LD) $(LDFLAGS)  -Map tftpwrite.map -o $@ $?

dnsip2name/start.o : dnsip2name/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

dnsip2name/u_dnsip2name.o: dnsip2name/u_dnsip2name.c
	$(CC) $(CFLAGS) -o $@ $<

../dns/dnsip2name : dnsip2name/start.o dnsip2name/u_dnsip2name.o $(LIB)
	$(LD) $(LDFLAGS)  -Map dnsip2name.map -o $@ $?

dnsname2ip/start.o : dnsname2ip/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

dnsname2ip/u_dnsname2ip.o: dnsname2ip/u_dnsname2ip.c
	$(CC) $(CFLAGS) -o $@ $<

../dns/dnsname2ip : dnsname2ip/start.o dnsname2ip/u_dnsname2ip.o $(LIB)
	$(LD) $(LDFLAGS)  -Map dnsname2ip.map -o $@ $?

dhcpdiscover/start.o : dhcpdiscover/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

dhcpdiscover/u_dhcpdiscover.o: dhcpdiscover/u_dhcpdiscover.c
	$(CC) $(CFLAGS) -o $@ $<

../dhcp/dhcpdiscover : dhcpdiscover/start.o dhcpdiscover/u_dhcpdiscover.o $(LIB)
	$(LD) $(LDFLAGS)  -Map dhcpdiscover.map -o $@ $?

dhcprequest/start.o : dhcprequest/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

dhcprequest/u_dhcprequest.o: dhcprequest/u_dhcprequest.c
	$(CC) $(CFLAGS) -o $@ $<

../dhcp/dhcprequest : dhcprequest/start.o dhcprequest/u_dhcprequest.o $(LIB)
	$(LD) $(LDFLAGS)  -Map dhcprequest.map -o $@ $?

dhcprelease/start.o : dhcprelease/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

dhcprelease/u_dhcprelease.o: dhcprelease/u_dhcprelease.c
	$(CC) $(CFLAGS) -o $@ $<

../dhcp/dhcprelease : dhcprelease/start.o dhcprelease/u_dhcprelease.o $(LIB)
	$(LD) $(LDFLAGS)  -Map dhcprelease.map -o $@ $?

snmpgetrequest/start.o : snmpgetrequest/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

snmpgetrequest/u_snmpgetrequest.o: snmpgetrequest/u_snmpgetrequest.c
	$(CC) $(CFLAGS) -o $@ $<

../snmp/snmpgetrequest : snmpgetrequest/start.o snmpgetrequest/u_snmpgetrequest.o $(LIB)
	$(LD) $(LDFLAGS)  -Map snmpgetrequest.map -o $@ $?

httpget/start.o : httpget/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

httpget/u_httpget.o: httpget/u_httpget.c
	$(CC) $(CFLAGS) -o $@ $<

../http/httpget : httpget/start.o httpget/u_httpget.o $(LIB)
	$(LD) $(LDFLAGS)  -Map httpget.map -o $@ $?

httpopen/start.o : httpopen/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

httpopen/u_httpopen.o: httpopen/u_httpopen.c
	$(CC) $(CFLAGS) -o $@ $<

../http/httpopen : httpopen/start.o httpopen/u_httpopen.o $(LIB)
	$(LD) $(LDFLAGS)  -Map httpopen.map -o $@ $?

smtpsend/start.o : smtpsend/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

smtpsend/u_smtpsend.o: smtpsend/u_smtpsend.c
	$(CC) $(CFLAGS) -o $@ $<

../smtp/smtpsend : smtpsend/start.o smtpsend/u_smtpsend.o $(LIB)
	$(LD) $(LDFLAGS)  -Map smtpsend.map -o $@ $?

smtpopen/start.o : smtpopen/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

smtpopen/u_smtpopen.o: smtpopen/u_smtpopen.c
	$(CC) $(CFLAGS) -o $@ $<

../smtp/smtpopen : smtpopen/start.o smtpopen/u_smtpopen.o $(LIB)
	$(LD) $(LDFLAGS)  -Map smtpopen.map -o $@ $?

pop3user/start.o : pop3user/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

pop3user/u_pop3user.o: pop3user/u_pop3user.c
	$(CC) $(CFLAGS) -o $@ $<

../pop3/pop3user : pop3user/start.o pop3user/u_pop3user.o $(LIB)
	$(LD) $(LDFLAGS)  -Map pop3user.map -o $@ $?

pop3open/start.o : pop3open/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

pop3open/u_pop3open.o: pop3open/u_pop3open.c
	$(CC) $(CFLAGS) -o $@ $<

../pop3/pop3open : pop3open/start.o pop3open/u_pop3open.o $(LIB)
	$(LD) $(LDFLAGS)  -Map pop3open.map -o $@ $?

telnetopen/start.o : telnetopen/start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

telnetopen/u_telnetopen.o: telnetopen/u_telnetopen.c
	$(CC) $(CFLAGS) -o $@ $<

../telnet/telnetopen : telnetopen/start.o telnetopen/u_telnetopen.o $(LIB)
	$(LD) $(LDFLAGS)  -Map telnetopen.map -o $@ $?
