#
# U-boot - config.mk
#
# Copyright (c) 2005 blackfin.uclinux.org
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

TEXT_BASE = 0x20000000
PLATFORM_CPPFLAGS += -I$(TOPDIR)

# OBJCFLAGS options to be given:
# --set-start=0x00 --adjust-section-vma=.text-$(TEXT_BASE)
#                  --adjust-section-vma=.text1-($(CFG_MONITOR_BASE) - XIP code size)
#                  --adjust-section-vma=.data-($(CFG_MONITOR_BASE) - XIP code size)
# where XIP code size is - size of instructions that got compiled to run from flash = .text code size
#                  This is same as the .text size outputed in u-boot.map
OBJCFLAGS = --set-start=0x00 --adjust-section-vma=.text-0x20000000 --adjust-section-vma=.text1-0x7f7fd58 --adjust-section-vma=.data-0x7f7fd58
