/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#define ASSEMBLY

#include <linux/config.h>
#include <asm/blackfin.h>
#include <config.h>

.global reset_board;
.global _reset_board;

reset_board:
_reset_board:
        /*Issue soft reset*/
	p0.h = SWRST >> 16;
	p0.l = SWRST & 0xFFFF;
	r0 = 0x0007;
	w[p0] = r0;
        ssync;

        p0.h = SWRST >> 16;
        p0.l = SWRST & 0xFFFF;
        r0 = 0x0000;
        w[p0] = r0;
        ssync;

        /*Clear soft reset*/
        raise 1;
        rts;


