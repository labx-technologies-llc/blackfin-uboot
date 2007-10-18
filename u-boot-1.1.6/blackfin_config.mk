# U-boot - blackfin_config.mk
#
# Copyright (c) 2005-2007 Analog Devices Inc.
#
# (C) Copyright 2000-2002
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

PLATFORM_RELFLAGS += -ffixed-P5 -fomit-frame-pointer
PLATFORM_CPPFLAGS += -DCONFIG_BLACKFIN

define extract_define
$(strip $(shell echo '$1' | $(CPP) -P -include $(TOPDIR)/include/configs/$(BOARD).h -I $(TOPDIR)/include - 2>/dev/null | tail -n 1))
endef

CONFIG_CPU := $(call extract_define, BFIN_CPU)
PLATFORM_RELFLAGS += -mcpu=$(CONFIG_CPU)

ENV_OFFSET_SIZE = $(call extract_define, $$((CFG_ENV_OFFSET)):$$((CFG_ENV_SIZE)))

BFIN_BOOT_MODE := $(shell \
	sed -n '/^\#define[[:space:]]*BFIN_BOOT_MODE[[:space:]]/s:.*[[:space:]]*BFIN_:BFIN_:p' \
	$(SRCTREE)/include/configs/$(BOARD).h)

LDR_FLAGS += --use-vmas
ifneq ($(BFIN_BOOT_MODE),BFIN_BOOT_BYPASS)
LDR_FLAGS += --initcode $(obj)cpu/$(CPU)/initcode.o
ifneq ($(BFIN_BOOT_MODE),BFIN_BOOT_UART)
LDR_FLAGS += --punchit $(ENV_OFFSET_SIZE):$(obj)env-ldr.o
endif
endif

ifneq ($(findstring s,$(MAKEFLAGS)),)
LDR_FLAGS += --quiet
endif
