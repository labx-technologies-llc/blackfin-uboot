/*
 * Rev:$Id$
 *
 * Copyright (C) 2003 Bas Vermeulen/Buyways B.V. <bf533@buyways.nl>
 *
 * Created by Buyways B.V. for Astent <info@astent.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA

**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       bf533_serial.h
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/


#ifndef _BF533_SERIAL_H_
#define _BF533_SERIAL_H_

#define BYTE_REF(addr)		(*((volatile char*)addr))
#define HALFWORD_REF(addr) 	(*((volatile short*)addr))
#define WORD_REF(addr)		(*((volatile long*)addr))

#define UART_THR_ADDR		0xffc00400
#define UART_THR(idx)		HALFWORD_REF(UART_THR_ADDR)

#define UART_RBR_ADDR		0xffc00400
#define UART_RBR(idx)		HALFWORD_REF(UART_RBR_ADDR)

#define UART_DLL_ADDR		0xffc00400
#define UART_DLL(idx)		HALFWORD_REF(UART_DLL_ADDR)

#define UART_IER_ADDR		0xffc00404
#define UART_IER(idx)		HALFWORD_REF(UART_IER_ADDR)
#define UART_IER_ERBFI		0x01
#define UART_IER_ETBEI		0x02
#define UART_IER_ELSI		0x04
#define UART_IER_EDDSI		0x08

#define UART_DLH_ADDR		0xffc00404
#define UART_DLH(idx)		HALFWORD_REF(UART_DLH_ADDR)
#define UART_IIR_ADDR		0xffc00408
#define UART_IIR(idx)		HALFWORD_REF(UART_IIR_ADDR)
#define UART_IIR_NOINT		0x01
#define UART_IIR_STATUS		0x06
#define UART_IIR_LSR		0x06
#define UART_IIR_RBR		0x04
#define UART_IIR_THR		0x02
#define UART_IIR_MSR		0x00

#define UART_LCR_ADDR          	0xffc0040C
#define UART_LCR(idx)          	HALFWORD_REF(UART_LCR_ADDR)
#define UART_LCR_WLS5           0
#define UART_LCR_WLS6           0x01
#define UART_LCR_WLS7           0x02
#define UART_LCR_WLS8           0x03
#define UART_LCR_STB            0x04
#define UART_LCR_PEN            0x08
#define UART_LCR_EPS            0x10
#define UART_LCR_SP             0x20
#define UART_LCR_SB             0x40
#define UART_LCR_DLAB           0x80

#define UART_MCR_ADDR          	0xffc00410
#define UART_MCR(idx)           HALFWORD_REF(UART_MCR_ADDR)

#define UART_LSR_ADDR          	0xffc00414
#define UART_LSR(idx)           HALFWORD_REF(UART_LSR_ADDR)
#define UART_LSR_DR             0x01
#define UART_LSR_OE             0x02
#define UART_LSR_PE             0x04
#define UART_LSR_FE             0x08
#define UART_LSR_BI             0x10
#define UART_LSR_THRE           0x20
#define UART_LSR_TEMT           0x40

#define UART_MSR_ADDR          	0xffc00418
#define UART_MSR(idx)           HALFWORD_REF(UART_MSR_ADDR)
#define UART_SCR_ADDR          	0xffc00418
#define UART_SCR(idx)           HALFWORD_REF(UART_SCR_ADDR)
#define UART_GCTL_ADDR		0xffc00424
#define UART_GCTL(idx)		HALFWORD_REF(UART_GCTL_ADDR)
#define UART_GCTL_UCEN		0x01

#endif /* _BF533_SERIAL_H_ */

