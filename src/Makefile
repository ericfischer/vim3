#		vim:ts=8:sw=8:
# Makefile for Vim on Unix
#

# Note: You MUST uncomment three hardware dependend lines!

# There are three types of defines:
#
# 1. configuration dependend
#	Used for "make install". Adjust the path names and protections
#	to your desire. Also defines the root for the X11 files (not required).
#
# 2. various choices
#	Can be changed to match your compiler or your preferences (not
#	required).
#
# 3. hardware dependend
#	If you machine is in the list, remove one '#' in front of the defines
#	following it. Otherwise: Find a machine that is similar and change the
#	defines to make it work. Normally you can just try and see what error
#	messages you get. (REQUIRED).

# The following systems have entries below. They have been tested and should
# work without modification. But later code changes may cause small problems.
# There are entries for other systems, but these have not been tested recently.

#system:		tested configurations:		tested by:

#Sun 4.1.x		cc	gcc	X11	no X11	(jw) (mool)
#FreeBSD		cc	gcc	X11	no X11	(mool)
#linux 1.0		cc		X11
#Linux 1.0.9			gcc		no X11	(jw)
#Apollo			cc			no X11	(webb)
#ULTRIX 4.2A on MIPS	cc	gcc		no X11	(mool)
#HPUX			cc	gcc	X11	no X11	(jw) (mool)
#irix 4.0.5H		cc		X11
#IRIX 4.0  SGI		cc	gcc	X11		(jw) (webb)
#SINIX-L 5.41		cc			no X11
#MOT188			cc			no X11
#Sequent/ptx 1.3	cc			no X11	(jw)
#osf1			cc		X11	no X11	(webb) (jw)
#Coherent 4.2.10	cc			no X11  (fredex@fcshome.stoneham.ma.us)
#Unisys 6035		cc			no X11
#SCO 3.2		cc	gcc		no X11	jos@oce.nl
#Solaris		cc		X11
#Solaris/Sun OS 5.3	cc			no X11	(jw)
#AIX (rs6000)		cc			no X11	(jw)
#RISCos on MIPS		cc		X11	no X11	(jw)

# configurations marked by (jw) have been tested by Juergen Weigert:
#    jnweiger@uni-erlangen.de

#
# PART 1: configuration dependend
#

### root directory for X11 files (unless overruled in hardware-dependend part)
### Unfortunately there is no standard for these, everybody puts them
### somewhere else
X11LIBDIR = /usr/openwin/lib
X11INCDIR = /usr/openwin/include
### for some hpux systems:
#X11LIBDIR = /usr/lib/X11R5
#X11INCDIR = /usr/include/X11R5

### Prefix for location of files
PREFIX = /usr/local

### Location of binary
BINLOC = $(PREFIX)/bin

### Name of target
TARGET = vim

### Location of man page
MANLOC = $(PREFIX)/man/man1

### Location of help file
HELPLOC = $(PREFIX)/lib

### Program to run on installed binary
STRIP = strip

### Permissions for vim binary
BINMOD = 755

### Permissions for man page
MANMOD = 644

### Permissions for help file
HELPMOD = 644

# Where to copy the man and help file from
HELPSOURCE = ../doc

#
# PART 2: various choices
#

### -DDIGRAPHS		digraph support
### -DNO_FREE_NULL	do not call free() with a null pointer
### -DCOMPATIBLE	start in vi-compatible mode
### -DNOBACKUP		default is no backup file
### -DDEBUG		output a lot of debugging garbage
### -DSTRNCASECMP	use strncasecmp() instead of internal function
### -DUSE_LOCALE	use setlocale() to change ctype() and others
### -DTERMCAP		full termcap/terminfo file support
### -DTERMINFO		use terminfo instead of termcap entries for builtin terms
### -DNO_BUILTIN_TCAPS	do not include builtin termcap entries
###				(use only with -DTERMCAP)
### -DSOME_BUILTIN_TCAPS include most useful builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -DALL_BUILTIN_TCAPS	include all builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -DMAXNAMLEN 31	maximum length of a file name (if not defined in sys/dir.h)
### -Dconst=		for compilers that don't have type const
### -DVIMRC_FILE=name		name of the .vimrc file in current dir
### -DEXRC_FILE=name		name of the .exrc file in current dir
### -DSYSVIMRC_FILE=name	name of the global .vimrc file
### -DSYSEXRC_FILE=name		name of the global .exrc file
### -DDEFVIMRC_FILE=name	name of the system-wide .vimrc file
### -DVIM_HLP=name		name of the help file
### -DUSE_SYSTEM	use system() instead of fork/exec for starting a shell
### -DVIM_ISSPACE	use when isspace() can't handle meta chars
### -DNOLIMITS		limits.h does not exist
### -DNOSTDLIB		stdlib.h does not exist
### -DUSE_X11		include code for xterm title saving
### -DNOTITLE		'title' option off by default
### -DVIMINFO		include reading/writing viminfo file
DEFS = -DDIGRAPHS -DTERMCAP -DSOME_BUILTIN_TCAPS -DNO_FREE_NULL -DVIM_ISSPACE \
		-DVIMINFO -DVIM_HLP=\"$(HELPLOC)/vim_help.txt\"

# Use this for cproto 3 patchlevel 6 or below (use "cproto -V" to check):
#     (maybe the "/usr/bin/cc -E" has to be adjusted for some systems)
# PROTO_FLAGS = -f4 -m__PARMS -d -E"/usr/bin/cc -E"

# Use this for cproto 3 patchlevel 7 or above (use "cproto -V" to check):
PROTO_FLAGS = -m -M__PARMS -d -E"/usr/bin/cc -E"

#
# PART 3: hardware dependend
#

### CC        - name and arguments for the compiler (also for linking)
### MACHINE   - defines used for compiling (not for linking)
### LIBS      - defines used for linking

# generic for Sun, NeXT, POSIX and SYSV R4 (?) (TESTED for Sun 4.1.x)
# standard cc with optimizer
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -Dconst= -DUSE_X11 -I$(X11INCDIR)
#CC=cc -O
#LIBS = -ltermlib -L$(X11LIBDIR) -lX11

MACHINE = -DBSD_UNIX -DUSE_LOCALE -Dconst=
CC=cc -O
LIBS = -ltermlib

# generic for Sun, FreeBSD, NetBSD, NeXT, POSIX and SYSV R4 (?) without x11 code
#	(TESTED for Sun 4.1.x and FreeBSD)
# standard cc with optimizer
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -Dconst=
#CC=cc -O
#LIBS = -ltermlib

# FreeBSD and NetBSD with Xfree (TESTED for FreeBSD)
# standard cc with optimizer
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -DUSE_X11 -I/usr/X386/include
#CC=cc -O
#LIBS = -ltermlib -L/usr/X386/lib -lX11

# FreeBSD and NetBSD with Xfree (TESTED for FreeBSD)
# gcc with optimizer or debugging
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -DUSE_X11 -I/usr/X386/include
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes
#CC=gcc -g -DDEBUG -Wall -Wshadow -Wmissing-prototypes
#LIBS = -ltermlib -L/usr/X386/lib -lX11

# like generic, but with termcap, for Linux, NeXT and others (NOT TESTED YET)
# standard cc with optimizer
#
#MACHINE = -DBSD_UNIX
#CC=cc -O
#LIBS = -ltermcap

# linux 1.0 with X11 (TESTED)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -DUSE_X11 -I/usr/X11/include
#CC=cc -O
#LIBS = -ltermcap -L/usr/X11/lib -lX11

# like generic, but with debugging (NOT TESTED YET)
#
#MACHINE = -DBSD_UNIX -g
#CC=cc
#LIBS = -ltermlib

# like generic, but with gcc and X11 (TESTED on Sun 4.1.x)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -DUSE_X11 -Dconst= -I$(X11INCDIR)
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes -traditional
#LIBS = -L$(X11LIBDIR) -ltermlib -lX11

# like generic, but with gcc, without X11 (TESTED on ULTRIX 4.2A on MIPS)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -Dconst=
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes -traditional
#LIBS = -ltermlib

# like generic, but with gcc 2.5.8 (TESTED on Sun 4.1.3_U1)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE
#CC=gcc -O1000
#LIBS = -ltermlib

# standard cc with optimizer for ULTRIX 4.2A on MIPS (ultrix defined) (TESTED)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE
#CC=cc -O -Olimit 1500
#LIBS = -ltermlib

# GCC (2.2.2d) on Linux (1.0.9) (TESTED)
#
#MACHINE = -DBSD_UNIX
#CC=gcc -O6 -Wall -Wshadow -Wmissing-prototypes
#LIBS = -ltermcap

# Apollo DOMAIN (with SYSTYPE = bsd4.3) (TESTED)
#
#MACHINE = -DBSD_UNIX -DDOMAIN
#CC=cc -O -A systype,bsd4.3
#LIBS = -ltermlib

# HPUX with X11 (TESTED) (hpux is defined)
#
#MACHINE = -DBSD_UNIX -DTERMINFO -DUSE_X11 -Dconst= -I$(X11INCDIR)
#CC=cc -O
#LIBS = -ltermcap -L$(X11LIBDIR) -lX11

# HPUX (TESTED) (hpux is defined)
#
#MACHINE = -DBSD_UNIX -DTERMINFO -Dconst=
#CC=cc -O
#LIBS = -ltermcap

# HPUX with gcc (TESTED) (hpux is defined)
#
#MACHINE = -DBSD_UNIX -DTERMINFO
#CC=gcc -O
#LIBS = -ltermcap

# hpux 9.01 (with termlib instead of termcap) (TESTED)
# irix 4.0.5H (TESTED)
#
#MACHINE = -DBSD_UNIX -DUSE_LOCALE -DUSE_X11 -I$(X11INCDIR)
#CC=cc -O
#LIBS = -ltermlib -L$(X11LIBDIR) -lX11

# IRIX 4.0 & 5.2 (Silicon Graphics Machines, __sgi will be defined) (TESTED)
#
#MACHINE = -DBSD_UNIX -DUSE_X11 -DTERMINFO
#CC=cc -O -Olimit 1500
#LIBS = -ltermlib -lX11 -lmalloc -lc_s

# IRIX 4.0 & 5.2 with gcc (Silicon Graphics Machines, __sgi will be defined)
# (TESTED)
#
#MACHINE = -DBSD_UNIX -DUSE_X11 -DTERMINFO
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes
#LIBS = -ltermlib -lX11 -lmalloc -lc_s

# Convex (NOT TESTED YET)
#
#MACHINE = -DBSD_UNIX -DCONVEX
#CC=cc -O
#LIBS = -ltermcap

# osf1 (alpha) with X11 (TESTED)
#
#MACHINE = -DSYSV_UNIX -DUSE_LOCALE -DUSE_X11
#CC=cc -O -Olimit 1500
#LIBS = -ltermlib -lX11

# generic SYSV_UNIX for Dynix/PTX and SYSV R3 (and R4?) (TESTED on SINIX-L 5.41)
# (TESTED on MOT188) (TESTED on Sequent/ptx 1.3) (TESTED on osf1)
# First try the line with locale. If this gives error messages try the other
# one.
#
#MACHINE = -DSYSV_UNIX -DUSE_LOCALE
#MACHINE = -DSYSV_UNIX
#CC=cc -O -Olimit 1500
#LIBS = -ltermlib

# generic SYSV_UNIX for Coherent 4.2.10
# (TESTED on Intel 386 platform)
#
#MACHINE = -DSYSV_UNIX -Dvoid=int
#CC=cc -O
#LIBS = -lterm -lsocket

# generic SYSV_UNIX with LOCALE (TESTED on Unisys 6035)
#
#MACHINE = -DSYSV_UNIX -DUSE_LOCALE -DUNISYS
#CC=cc -O
#LIBS = -ltermlib

# SCO Xenix (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -DSCO
#CC=cc -O
#LIBS = -ltermlib

# GCC on SCO 3.2 (TESTED by jos@oce.nl)
# cc works too.
#
#MACHINE = -DSYSV_UNIX -UM_XENIX -DSCO
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes
#LIBS = -ltinfo

# GCC on another SCO Unix (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -UM_XENIX -DSCO -g
#CC=gcc -O6 -fpcc-struct-return -fwritable-strings
#LIBS = -ltermlib -lmalloc

# Dynix with gcc (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX
#CC=gcc -O -Wall -Wshadow -Wmissing-prototypes -traditional
#LIBS = -ltermlib

# SOLARIS with X11 anc cc (TESTED)
#
#MACHINE = -DSYSV_UNIX -DSOLARIS -DTERMINFO -DUSE_X11 -I$(X11INCDIR)
#CC=cc -O -Xa -v -R$(X11LIBDIR)
#LIBS = -L$(X11LIBDIR) -ltermlib -lX11

# SOLARIS with X11 and gcc (TESTED with SOLARIS 2.3 and gcc 2.5.8)
#
#MACHINE = -DSYSV_UNIX -DSOLARIS -DTERMINFO -DUSE_X11 -I$(X11INCDIR)
#CC=gcc -O -R$(X11LIBDIR)
#LIBS = -L$(X11LIBDIR) -ltermlib -lX11

# SOLARIS (also works for Esix 4.0.3, SYSV R4?) (TESTED on Sun OS 5.3)
#
#MACHINE = -DSYSV_UNIX -DSOLARIS -DTERMINFO
#CC=cc -O -Xa -v
#LIBS = -ltermlib

# UNICOS (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -DUNICOS
#CC=cc -O
#LIBS = -ltermlib

# AIX (rs6000) (TESTED)
#
#MACHINE = -DSYSV_UNIX -DAIX
#CC=cc -O
#LIBS=-lcur

# UTS2 for Amdahl UTS 2.1.x (disable termcap below) (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -DUTS2
#CC=cc -O
#LIBS = -ltermlib -lsocket

# UTS4 for Amdahl UTS 4.x (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -DUTS4 -Xa
#CC=cc -O
#LIBS = -ltermlib

# USL for Unix Systems Laboratories (SYSV 4.2) (NOT TESTED YET)
#
#MACHINE = -DSYSV_UNIX -DUSL
#CC=cc -O
#LIBS = -ltermlib

# RISCos on MIPS without X11 (TESTED)
#
#MACHINE = -DSYSV_UNIX -DMIPS
#CC=cc -O
#LIBS = -ltermlib

# RISCos on MIPS with X11 (TESTED)
#
#MACHINE=-DSYSV_UNIX -DUSE_LOCALE -DUSE_X11 -I$(X11INCDIR)
#CC=cc -O
#LIBS=-ltermlib -L$(X11LIBDIR) -lX11 -lsun

################################################
##   no changes required below this line      ##
################################################

CFLAGS = -c $(MACHINE) $(DEFS)

PFLAGS = $(PROTO_FLAGS) -DPROTO $(MACHINE) $(DEFS)

INCL = vim.h globals.h param.h keymap.h macros.h ascii.h term.h unix.h structs.h proto.h

SRC =	alloc.c buffer.c charset.c \
	cmdcmds.c cmdline.c csearch.c \
	digraph.c edit.c fileio.c getchar.c \
	help.c linefunc.c main.c mark.c \
	memfile.c memline.c message.c \
	misccmds.c normal.c ops.c param.c \
	quickfix.c regexp.c regsub.c screen.c \
	search.c tag.c term.c undo.c \
	unix.c window.c version.c

OBJ =	objects/alloc.o objects/unix.o objects/buffer.o objects/charset.o \
	objects/cmdcmds.o objects/cmdline.o objects/csearch.o \
	objects/digraph.o objects/edit.o objects/fileio.o objects/getchar.o \
	objects/help.o objects/linefunc.o objects/main.o objects/mark.o \
	objects/memfile.o objects/memline.o objects/message.o \
	objects/misccmds.o objects/normal.o objects/ops.o objects/param.o \
	objects/quickfix.o objects/regexp.o objects/regsub.o objects/screen.o \
	objects/search.o objects/tag.o objects/term.o objects/undo.o \
	objects/window.o $(TERMLIB)

PRO =	proto/alloc.pro proto/buffer.pro proto/charset.pro proto/cmdcmds.pro \
	proto/cmdline.pro proto/csearch.pro proto/digraph.pro proto/edit.pro \
	proto/fileio.pro proto/getchar.pro proto/help.pro proto/linefunc.pro \
	proto/main.pro proto/mark.pro proto/memfile.pro proto/memline.pro \
	proto/message.pro proto/misccmds.pro proto/normal.pro proto/ops.pro \
	proto/param.pro proto/quickfix.pro proto/regexp.pro proto/regsub.pro \
	proto/screen.pro proto/search.pro proto/tag.pro proto/term.pro \
	proto/termlib.pro proto/undo.pro proto/window.pro proto/amiga.pro \
	proto/unix.pro

$(TARGET): objects $(OBJ) version.c
	$(CC) $(CFLAGS) version.c
	mv version.o objects/version.o
	$(CC) -o $(TARGET) $(OBJ) objects/version.o $(LIBS)

debug: objects $(OBJ) version.c
	$(CC) $(CFLAGS) version.c
	mv version.o objects/version.o
	$(CC) -o $(TARGET) -g $(OBJ) objects/version.o $(LIBS)

objects:
	-mkdir objects

# Generate function prototypes.  This is not needed to compile vim, but if
# you want to use it, cproto is out there on the net somewhere -- Webb
#
# When generating msdos.pro and amiga.pro there will be a few include
# files that can not be found, that's OK.
proto: $(PRO)

proto/msdos.pro: msdos.c
	cproto $(PFLAGS) -DMSDOS -UBSD_UNIX msdos.c > proto/msdos.pro

proto/amiga.pro: amiga.c
	cproto $(PFLAGS) -DAMIGA -UBSD_UNIX -DBPTR=char* amiga.c > proto/amiga.pro

ctags:
# simplistic ctags
#	ctags -t $(SRC) $(INCL)
# include static functions (not all versions of ctags can do this)
	ctags -ts $(SRC) $(INCL)
# also include variables (not all versions of ctags can do this)
#	ctags -tvs $(SRC) $(INCL)

install: $(TARGET)
	-mkdir $(BINLOC)
	cp $(TARGET) $(BINLOC)
	$(STRIP) $(BINLOC)/$(TARGET)
	chmod $(BINMOD) $(BINLOC)/$(TARGET)
	-mkdir $(MANLOC)
	cp $(HELPSOURCE)/vim.1 $(MANLOC)
	chmod $(MANMOD) $(MANLOC)/vim.1
	-mkdir $(HELPLOC)
	cp $(HELPSOURCE)/vim_*.txt $(HELPLOC)
	cp $(HELPSOURCE)/vim_tags $(HELPLOC)
	chmod $(HELPMOD) $(HELPLOC)/vim_*.txt $(HELPLOC)/vim_tags

clean:
	-rm -f $(OBJ) objects/mkcmdtab.o objects/vim.o objects/version.o core vim.core $(TARGET) mkcmdtab cmdtab.h
	-rm -f *.bak

#use this in case the files have been transported via an MSDOS system

FILES = *.c *.h makefile makefile.* cmdtab.tab proto/*.pro tags

dos2unix:
	-mv arp_prot.h arp_proto.h
	-mv ptx_stdl.h ptx_stdlib.h
	-mv sun_stdl.h sun_stdlib.h
	-mv makefile.dic makefile.dice
	-mv makefile.uni makefile.unix
	-mv makefile.man makefile.manx
	-mv makefile.6sa makefile.6sas
	-mv makefile.5sa makefile.5sas
	for i in $(FILES); do tr -d '\r\032' < $$i > ~tmp~; mv ~tmp~ $$i; echo $$i; done

###########################################################################

objects/alloc.o:	alloc.c  $(INCL)
	$(CC) $(CFLAGS) alloc.c
	mv alloc.o objects/alloc.o

proto/alloc.pro:	alloc.c
	cproto $(PFLAGS) alloc.c >proto/alloc.pro

objects/unix.o:	unix.c  $(INCL)
	$(CC) $(CFLAGS) unix.c
	mv unix.o objects/unix.o

proto/unix.pro:		unix.c
	cproto $(PFLAGS) unix.c >proto/unix.pro

objects/buffer.o:	buffer.c  $(INCL)
	$(CC) $(CFLAGS) buffer.c
	mv buffer.o objects/buffer.o

proto/buffer.pro:	buffer.c
	cproto $(PFLAGS) buffer.c >proto/buffer.pro

objects/charset.o:	charset.c  $(INCL)
	$(CC) $(CFLAGS) charset.c
	mv charset.o objects/charset.o

proto/charset.pro:	charset.c
	cproto $(PFLAGS) charset.c >proto/charset.pro

objects/cmdcmds.o:	cmdcmds.c  $(INCL)
	$(CC) $(CFLAGS) cmdcmds.c
	mv cmdcmds.o objects/cmdcmds.o

proto/cmdcmds.pro:	cmdcmds.c
	cproto $(PFLAGS) cmdcmds.c >proto/cmdcmds.pro

objects/cmdline.o:	cmdline.c  $(INCL) cmdtab.h ops.h
	$(CC) $(CFLAGS) cmdline.c
	mv cmdline.o objects/cmdline.o

proto/cmdline.pro:	cmdline.c
	cproto $(PFLAGS) cmdline.c >proto/cmdline.pro

objects/csearch.o:	csearch.c  $(INCL)
	$(CC) $(CFLAGS) csearch.c
	mv csearch.o objects/csearch.o

proto/csearch.pro:	csearch.c
	cproto $(PFLAGS) csearch.c >proto/csearch.pro

objects/digraph.o:	digraph.c  $(INCL)
	$(CC) $(CFLAGS) digraph.c
	mv digraph.o objects/digraph.o

proto/digraph.pro:	digraph.c
	cproto $(PFLAGS) digraph.c >proto/digraph.pro

objects/edit.o:	edit.c  $(INCL) ops.h
	$(CC) $(CFLAGS) edit.c
	mv edit.o objects/edit.o

proto/edit.pro:	edit.c
	cproto $(PFLAGS) edit.c >proto/edit.pro

objects/fileio.o:	fileio.c  $(INCL)
	$(CC) $(CFLAGS) fileio.c
	mv fileio.o objects/fileio.o

proto/fileio.pro:	fileio.c
	cproto $(PFLAGS) fileio.c >proto/fileio.pro

objects/getchar.o:	getchar.c  $(INCL)
	$(CC) $(CFLAGS) getchar.c
	mv getchar.o objects/getchar.o

proto/getchar.pro:	getchar.c
	cproto $(PFLAGS) getchar.c >proto/getchar.pro

objects/help.o:	help.c  $(INCL)
	$(CC) $(CFLAGS) help.c
	mv help.o objects/help.o

proto/help.pro:	help.c
	cproto $(PFLAGS) help.c >proto/help.pro

objects/linefunc.o:	linefunc.c  $(INCL)
	$(CC) $(CFLAGS) linefunc.c
	mv linefunc.o objects/linefunc.o

proto/linefunc.pro:	linefunc.c
	cproto $(PFLAGS) linefunc.c >proto/linefunc.pro

objects/main.o:	main.c  $(INCL)
	$(CC) $(CFLAGS) main.c
	mv main.o objects/main.o

proto/main.pro:	main.c
	cproto $(PFLAGS) main.c >proto/main.pro

objects/mark.o:	mark.c  $(INCL)
	$(CC) $(CFLAGS) mark.c
	mv mark.o objects/mark.o

proto/mark.pro:	mark.c
	cproto $(PFLAGS) mark.c >proto/mark.pro

objects/memfile.o:	memfile.c  $(INCL)
	$(CC) $(CFLAGS) memfile.c
	mv memfile.o objects/memfile.o

proto/memfile.pro:	memfile.c
	cproto $(PFLAGS) memfile.c >proto/memfile.pro

objects/memline.o:	memline.c  $(INCL)
	$(CC) $(CFLAGS) memline.c
	mv memline.o objects/memline.o

proto/memline.pro:	memline.c
	cproto $(PFLAGS) memline.c >proto/memline.pro

objects/message.o:	message.c  $(INCL)
	$(CC) $(CFLAGS) message.c
	mv message.o objects/message.o

proto/message.pro:	message.c
	cproto $(PFLAGS) message.c >proto/message.pro

objects/misccmds.o:	misccmds.c  $(INCL)
	$(CC) $(CFLAGS) misccmds.c
	mv misccmds.o objects/misccmds.o

proto/misccmds.pro:	misccmds.c
	cproto $(PFLAGS) misccmds.c >proto/misccmds.pro

objects/normal.o:	normal.c  $(INCL) ops.h
	$(CC) $(CFLAGS) normal.c
	mv normal.o objects/normal.o

proto/normal.pro:	normal.c
	cproto $(PFLAGS) normal.c >proto/normal.pro

objects/ops.o:	ops.c  $(INCL) ops.h
	$(CC) $(CFLAGS) ops.c
	mv ops.o objects/ops.o

proto/ops.pro:	ops.c
	cproto $(PFLAGS) ops.c >proto/ops.pro

objects/param.o:	param.c  $(INCL)
	$(CC) $(CFLAGS) param.c
	mv param.o objects/param.o

proto/param.pro:	param.c
	cproto $(PFLAGS) param.c >proto/param.pro

objects/quickfix.o:	quickfix.c  $(INCL)
	$(CC) $(CFLAGS) quickfix.c
	mv quickfix.o objects/quickfix.o

proto/quickfix.pro:	quickfix.c
	cproto $(PFLAGS) quickfix.c >proto/quickfix.pro

objects/regexp.o:	regexp.c  $(INCL)
	$(CC) $(CFLAGS) regexp.c
	mv regexp.o objects/regexp.o

proto/regexp.pro:	regexp.c
	cproto $(PFLAGS) regexp.c >proto/regexp.pro

objects/regsub.o:	regsub.c  $(INCL)
	$(CC) $(CFLAGS) regsub.c
	mv regsub.o objects/regsub.o

proto/regsub.pro:	regsub.c
	cproto $(PFLAGS) regsub.c >proto/regsub.pro

objects/screen.o:	screen.c  $(INCL)
	$(CC) $(CFLAGS) screen.c
	mv screen.o objects/screen.o

proto/screen.pro:	screen.c
	cproto $(PFLAGS) screen.c >proto/screen.pro

objects/search.o:	search.c  $(INCL) ops.h
	$(CC) $(CFLAGS) search.c
	mv search.o objects/search.o

proto/search.pro:	search.c
	cproto $(PFLAGS) search.c >proto/search.pro

objects/tag.o:	tag.c  $(INCL)
	$(CC) $(CFLAGS) tag.c
	mv tag.o objects/tag.o

proto/tag.pro:	tag.c
	cproto $(PFLAGS) tag.c >proto/tag.pro

objects/term.o:	term.c  $(INCL)
	$(CC) $(CFLAGS) term.c
	mv term.o objects/term.o

proto/term.pro:	term.c
	cproto $(PFLAGS) term.c >proto/term.pro

objects/undo.o:	undo.c  $(INCL)
	$(CC) $(CFLAGS) undo.c
	mv undo.o objects/undo.o

proto/undo.pro:	undo.c
	cproto $(PFLAGS) undo.c >proto/undo.pro

objects/window.o:	window.c  $(INCL)
	$(CC) $(CFLAGS) window.c
	mv window.o objects/window.o

proto/window.pro:	window.c
	cproto $(PFLAGS) window.c >proto/window.pro

cmdtab.h: cmdtab.tab mkcmdtab
	./mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab: mkcmdtab.c
	$(CC) $(MACHINE) $(DEFS) -o mkcmdtab mkcmdtab.c
