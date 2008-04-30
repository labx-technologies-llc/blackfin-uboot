#
# Copyright (c) 2005-2008 Analog Device Inc.
#
# (C) Copyright 2001
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# Licensed under the GPL-2 or later.
#

# This is not actually used for Blackfin boards so do not change it
#TEXT_BASE = do-not-use-me

# Set some default LDR flags based on boot mode.
LDR_FLAGS-BFIN_BOOT_PARA := --bits 16 --dma 8
LDR_FLAGS += $(LDR_FLAGS-$(CONFIG_BFIN_BOOT_MODE))
