#
# (C) Copyright 2000-2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

ifeq ($(ARCH),ppc)
LOAD_ADDR = 0x40000
endif

ifeq ($(ARCH),i386)
LOAD_ADDR = 0x40000
endif

ifeq ($(ARCH),arm)
LOAD_ADDR = 0xc100000
endif

ifeq ($(ARCH),mips)
LOAD_ADDR = 0x80200000 -T mips.lds
endif

ifeq ($(ARCH),nios)
LOAD_ADDR = 0x00800000 -L $(gcclibdir)/m32 -T nios.lds
endif

ifeq ($(ARCH),nios2)
LOAD_ADDR = 0x00800000 -L $(gcclibdir) -T nios2.lds
endif

ifeq ($(ARCH),m68k)
LOAD_ADDR = 0x20000  -L $(clibdir)
endif

ifeq ($(ARCH),microblaze)
LOAD_ADDR = 0x80F00000
endif

ifeq ($(ARCH),blackfin)
LOAD_ADDR = 0x1000
endif

include $(TOPDIR)/config.mk

SREC	= hello_world.srec
BIN	= hello_world hello_world.bin

ifeq ($(ARCH),i386)
SREC   += 82559_eeprom.srec
BIN    += 82559_eeprom 82559_eeprom.bin
endif

ifeq ($(ARCH),ppc)
SREC   += sched.srec
BIN    += sched sched.bin
endif

ifeq ($(ARCH),blackfin)
ifneq ($(BOARD),bf537)  
ifneq ($(BOARD),cm-bf537) 
ifneq ($(BOARD),serial_bf537)
SREC	+= smc91111_eeprom.srec
BIN 	+= smc91111_eeprom smc91111_eeprom.bin
endif
endif
endif
endif

# The following example is pretty 8xx specific...
ifeq ($(CPU),mpc8xx)
SREC   += timer.srec
BIN    += timer timer.bin
endif

# The following example is 8260 specific...
ifeq ($(CPU),mpc8260)
SREC   += mem_to_mem_idma2intr.srec
BIN    += mem_to_mem_idma2intr mem_to_mem_idma2intr.bin
endif

# Utility for resetting i82559 EEPROM
ifeq ($(BOARD),oxc)
SREC   += eepro100_eeprom.srec
BIN    += eepro100_eeprom eepro100_eeprom.bin
endif

ifeq ($(BIG_ENDIAN),y)
EX_LDFLAGS += -EB
endif

OBJS	= $(SREC:.srec=.o)

LIB	= libstubs.a
LIBAOBJS=
ifeq ($(ARCH),ppc)
LIBAOBJS+= $(ARCH)_longjmp.o $(ARCH)_setjmp.o
endif
LIBCOBJS= stubs.o
LIBOBJS	= $(LIBAOBJS) $(LIBCOBJS)

gcclibdir := $(shell dirname `$(CC) -print-libgcc-file-name`)
clibdir := $(shell dirname `$(CC) $(CFLAGS) -print-file-name=libc.a`)

CPPFLAGS += -I..

all:	.depend $(OBJS) $(LIB) $(BIN) $(SREC)

#########################################################################
$(LIB): .depend $(LIBOBJS)
	$(AR) cr $@ $(LIBOBJS)

ifeq ($(ARCH),blackfin)
%:      %.o $(LIB)
	$(LD) -g $(EX_LDFLAGS) -Ttext $(LOAD_ADDR) \
		-o $@ -e _$(<:.o=) $< $(LIB) \
		-L$(gcclibdir) -lgcc
else
%:	%.o $(LIB)
	$(LD) -g $(EX_LDFLAGS) -Ttext $(LOAD_ADDR) \
		-o $@ -e $(<:.o=) $< $(LIB) \
		-L$(gcclibdir) -lgcc
endif

%.srec:	%
	$(OBJCOPY) -O srec $< $@ 2>/dev/null

%.bin:	%
	$(OBJCOPY) -O binary $< $@ 2>/dev/null

#########################################################################

.depend:	Makefile $(OBJS:.o=.c) $(LIBCOBJS:.o=.c) $(LIBAOBJS:.o=.S)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) $(LIBCOBJS:.o=.c) $(LIBAOBJS:.o=.S) > $@

sinclude .depend

#########################################################################
