# U-boot - Makefile
#
# Copyright (c) 2005-2007 Analog Devices Inc.
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
# MA 02110-1301 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(CPU).a

EXTRA    :=
CEXTRA   := initcode.o
SEXTRA   := start.o
SOBJS    := interrupt.o cache.o flush.o
COBJS    := cpu.o traps.o interrupts.o reset.o serial.o video.o i2c.o watchdog.o

ifeq ($(BFIN_BOOT_MODE),BFIN_BOOT_BYPASS)
COBJS    += initcode.o
endif

SRCS     := $(SEXTRA:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS     := $(addprefix $(obj),$(COBJS) $(SOBJS))
EXTRA    := $(addprefix $(obj),$(EXTRA))
CEXTRA   := $(addprefix $(obj),$(CEXTRA))
SEXTRA   := $(addprefix $(obj),$(SEXTRA))

all:	$(obj).depend $(LIB) $(obj).depend $(EXTRA) $(CEXTRA) $(SEXTRA) check_initcode

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(OBJS): $(obj)bootrom-asm-offsets.h
$(obj)bootrom-asm-offsets.c: bootrom-asm-offsets.c.in bootrom-asm-offsets.awk
	echo '#include <asm/mach-common/bits/bootrom.h>' | $(CPP) $(CPPFLAGS) - | ./bootrom-asm-offsets.awk > $@.tmp
	mv $@.tmp $@
$(obj)bootrom-asm-offsets.s: $(obj)bootrom-asm-offsets.c
	$(CC) $(CFLAGS) -S $^ -o $@.tmp
	mv $@.tmp $@
$(obj)bootrom-asm-offsets.h: $(obj)bootrom-asm-offsets.s
	sed -ne "/^->/{s:^->\([^ ]*\) [\$$#]*\([^ ]*\) \(.*\):#define \1 \2 /* \3 */:; s:->::; p;}" $^ > $@

# make sure our initcode (which goes into LDR) does not
# have relocs or external references
READINIT = env LC_ALL=C $(CROSS_COMPILE)readelf -s $<
check_initcode: $(obj)initcode.o
ifneq ($(BFIN_BOOT_MODE),BFIN_BOOT_BYPASS)
	@if $(READINIT) | grep '\<GLOBAL\>.*\<UND\>' ; then \
		echo "$< contains external references!" 1>&2 ; \
		exit 1 ; \
	fi
endif

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
