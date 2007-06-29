#
# (C) Copyright 2001
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

# This is not actually used for Blackfin boards so do not change it
#TEXT_BASE = do-not-use-me

# Set some default LDR flags based on boot mode.
BOOT_MODE := $(shell \
	sed -n '/^\#define[[:space:]]*BFIN_BOOT_MODE[[:space:]]/s:.*[[:space:]]*BFIN_:BFIN_:p' \
	$(SRCTREE)/include/configs/bf548-ezkit.h)
LDR_FLAGS-BFIN_PARA_BOOT       := --dma 6
LDR_FLAGS-BFIN_FIFO_BOOT       := --dma 1
LDR_FLAGS-BFIN_SPI_MASTER_BOOT := --dma 1
LDR_FLAGS-BFIN_UART_BOOT       := --dma 1
LDR_FLAGS += $(LDR_FLAGS-$(BOOT_MODE))
