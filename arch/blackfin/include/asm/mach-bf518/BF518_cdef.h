/* DO NOT EDIT THIS FILE
 * Automatically generated by generate-cdef-headers.xsl
 * DO NOT EDIT THIS FILE
 */

#ifndef __BFIN_CDEF_ADSP_BF518_proc__
#define __BFIN_CDEF_ADSP_BF518_proc__

#include "../mach-common/ADSP-EDN-core_cdef.h"

#include "ADSP-EDN-BF51x-extended_cdef.h"

#define bfin_read_PLL_CTL()            bfin_read16(PLL_CTL)
#define bfin_write_PLL_CTL(val)        bfin_write16(PLL_CTL, val)
#define bfin_read_PLL_DIV()            bfin_read16(PLL_DIV)
#define bfin_write_PLL_DIV(val)        bfin_write16(PLL_DIV, val)
#define bfin_read_VR_CTL()             bfin_read16(VR_CTL)
#define bfin_write_VR_CTL(val)         bfin_write16(VR_CTL, val)
#define bfin_read_PLL_STAT()           bfin_read16(PLL_STAT)
#define bfin_write_PLL_STAT(val)       bfin_write16(PLL_STAT, val)
#define bfin_read_PLL_LOCKCNT()        bfin_read16(PLL_LOCKCNT)
#define bfin_write_PLL_LOCKCNT(val)    bfin_write16(PLL_LOCKCNT, val)
#define bfin_read_CHIPID()             bfin_read32(CHIPID)
#define bfin_write_CHIPID(val)         bfin_write32(CHIPID, val)
#define bfin_read_SWRST()              bfin_read16(SWRST)
#define bfin_write_SWRST(val)          bfin_write16(SWRST, val)
#define bfin_read_SYSCR()              bfin_read16(SYSCR)
#define bfin_write_SYSCR(val)          bfin_write16(SYSCR, val)
#define bfin_read_SRAM_BASE_ADDR()     bfin_readPTR(SRAM_BASE_ADDR)
#define bfin_write_SRAM_BASE_ADDR(val) bfin_writePTR(SRAM_BASE_ADDR, val)
#define bfin_read_DMEM_CONTROL()       bfin_read32(DMEM_CONTROL)
#define bfin_write_DMEM_CONTROL(val)   bfin_write32(DMEM_CONTROL, val)
#define bfin_read_DCPLB_STATUS()       bfin_read32(DCPLB_STATUS)
#define bfin_write_DCPLB_STATUS(val)   bfin_write32(DCPLB_STATUS, val)
#define bfin_read_DCPLB_FAULT_ADDR()   bfin_readPTR(DCPLB_FAULT_ADDR)
#define bfin_write_DCPLB_FAULT_ADDR(val) bfin_writePTR(DCPLB_FAULT_ADDR, val)
#define bfin_read_DCPLB_ADDR0()        bfin_readPTR(DCPLB_ADDR0)
#define bfin_write_DCPLB_ADDR0(val)    bfin_writePTR(DCPLB_ADDR0, val)
#define bfin_read_DCPLB_ADDR1()        bfin_readPTR(DCPLB_ADDR1)
#define bfin_write_DCPLB_ADDR1(val)    bfin_writePTR(DCPLB_ADDR1, val)
#define bfin_read_DCPLB_ADDR2()        bfin_readPTR(DCPLB_ADDR2)
#define bfin_write_DCPLB_ADDR2(val)    bfin_writePTR(DCPLB_ADDR2, val)
#define bfin_read_DCPLB_ADDR3()        bfin_readPTR(DCPLB_ADDR3)
#define bfin_write_DCPLB_ADDR3(val)    bfin_writePTR(DCPLB_ADDR3, val)
#define bfin_read_DCPLB_ADDR4()        bfin_readPTR(DCPLB_ADDR4)
#define bfin_write_DCPLB_ADDR4(val)    bfin_writePTR(DCPLB_ADDR4, val)
#define bfin_read_DCPLB_ADDR5()        bfin_readPTR(DCPLB_ADDR5)
#define bfin_write_DCPLB_ADDR5(val)    bfin_writePTR(DCPLB_ADDR5, val)
#define bfin_read_DCPLB_ADDR6()        bfin_readPTR(DCPLB_ADDR6)
#define bfin_write_DCPLB_ADDR6(val)    bfin_writePTR(DCPLB_ADDR6, val)
#define bfin_read_DCPLB_ADDR7()        bfin_readPTR(DCPLB_ADDR7)
#define bfin_write_DCPLB_ADDR7(val)    bfin_writePTR(DCPLB_ADDR7, val)
#define bfin_read_DCPLB_ADDR8()        bfin_readPTR(DCPLB_ADDR8)
#define bfin_write_DCPLB_ADDR8(val)    bfin_writePTR(DCPLB_ADDR8, val)
#define bfin_read_DCPLB_ADDR9()        bfin_readPTR(DCPLB_ADDR9)
#define bfin_write_DCPLB_ADDR9(val)    bfin_writePTR(DCPLB_ADDR9, val)
#define bfin_read_DCPLB_ADDR10()       bfin_readPTR(DCPLB_ADDR10)
#define bfin_write_DCPLB_ADDR10(val)   bfin_writePTR(DCPLB_ADDR10, val)
#define bfin_read_DCPLB_ADDR11()       bfin_readPTR(DCPLB_ADDR11)
#define bfin_write_DCPLB_ADDR11(val)   bfin_writePTR(DCPLB_ADDR11, val)
#define bfin_read_DCPLB_ADDR12()       bfin_readPTR(DCPLB_ADDR12)
#define bfin_write_DCPLB_ADDR12(val)   bfin_writePTR(DCPLB_ADDR12, val)
#define bfin_read_DCPLB_ADDR13()       bfin_readPTR(DCPLB_ADDR13)
#define bfin_write_DCPLB_ADDR13(val)   bfin_writePTR(DCPLB_ADDR13, val)
#define bfin_read_DCPLB_ADDR14()       bfin_readPTR(DCPLB_ADDR14)
#define bfin_write_DCPLB_ADDR14(val)   bfin_writePTR(DCPLB_ADDR14, val)
#define bfin_read_DCPLB_ADDR15()       bfin_readPTR(DCPLB_ADDR15)
#define bfin_write_DCPLB_ADDR15(val)   bfin_writePTR(DCPLB_ADDR15, val)
#define bfin_read_DCPLB_DATA0()        bfin_read32(DCPLB_DATA0)
#define bfin_write_DCPLB_DATA0(val)    bfin_write32(DCPLB_DATA0, val)
#define bfin_read_DCPLB_DATA1()        bfin_read32(DCPLB_DATA1)
#define bfin_write_DCPLB_DATA1(val)    bfin_write32(DCPLB_DATA1, val)
#define bfin_read_DCPLB_DATA2()        bfin_read32(DCPLB_DATA2)
#define bfin_write_DCPLB_DATA2(val)    bfin_write32(DCPLB_DATA2, val)
#define bfin_read_DCPLB_DATA3()        bfin_read32(DCPLB_DATA3)
#define bfin_write_DCPLB_DATA3(val)    bfin_write32(DCPLB_DATA3, val)
#define bfin_read_DCPLB_DATA4()        bfin_read32(DCPLB_DATA4)
#define bfin_write_DCPLB_DATA4(val)    bfin_write32(DCPLB_DATA4, val)
#define bfin_read_DCPLB_DATA5()        bfin_read32(DCPLB_DATA5)
#define bfin_write_DCPLB_DATA5(val)    bfin_write32(DCPLB_DATA5, val)
#define bfin_read_DCPLB_DATA6()        bfin_read32(DCPLB_DATA6)
#define bfin_write_DCPLB_DATA6(val)    bfin_write32(DCPLB_DATA6, val)
#define bfin_read_DCPLB_DATA7()        bfin_read32(DCPLB_DATA7)
#define bfin_write_DCPLB_DATA7(val)    bfin_write32(DCPLB_DATA7, val)
#define bfin_read_DCPLB_DATA8()        bfin_read32(DCPLB_DATA8)
#define bfin_write_DCPLB_DATA8(val)    bfin_write32(DCPLB_DATA8, val)
#define bfin_read_DCPLB_DATA9()        bfin_read32(DCPLB_DATA9)
#define bfin_write_DCPLB_DATA9(val)    bfin_write32(DCPLB_DATA9, val)
#define bfin_read_DCPLB_DATA10()       bfin_read32(DCPLB_DATA10)
#define bfin_write_DCPLB_DATA10(val)   bfin_write32(DCPLB_DATA10, val)
#define bfin_read_DCPLB_DATA11()       bfin_read32(DCPLB_DATA11)
#define bfin_write_DCPLB_DATA11(val)   bfin_write32(DCPLB_DATA11, val)
#define bfin_read_DCPLB_DATA12()       bfin_read32(DCPLB_DATA12)
#define bfin_write_DCPLB_DATA12(val)   bfin_write32(DCPLB_DATA12, val)
#define bfin_read_DCPLB_DATA13()       bfin_read32(DCPLB_DATA13)
#define bfin_write_DCPLB_DATA13(val)   bfin_write32(DCPLB_DATA13, val)
#define bfin_read_DCPLB_DATA14()       bfin_read32(DCPLB_DATA14)
#define bfin_write_DCPLB_DATA14(val)   bfin_write32(DCPLB_DATA14, val)
#define bfin_read_DCPLB_DATA15()       bfin_read32(DCPLB_DATA15)
#define bfin_write_DCPLB_DATA15(val)   bfin_write32(DCPLB_DATA15, val)
#define bfin_read_DTEST_COMMAND()      bfin_read32(DTEST_COMMAND)
#define bfin_write_DTEST_COMMAND(val)  bfin_write32(DTEST_COMMAND, val)
#define bfin_read_DTEST_DATA0()        bfin_read32(DTEST_DATA0)
#define bfin_write_DTEST_DATA0(val)    bfin_write32(DTEST_DATA0, val)
#define bfin_read_DTEST_DATA1()        bfin_read32(DTEST_DATA1)
#define bfin_write_DTEST_DATA1(val)    bfin_write32(DTEST_DATA1, val)
#define bfin_read_IMEM_CONTROL()       bfin_read32(IMEM_CONTROL)
#define bfin_write_IMEM_CONTROL(val)   bfin_write32(IMEM_CONTROL, val)
#define bfin_read_ICPLB_STATUS()       bfin_read32(ICPLB_STATUS)
#define bfin_write_ICPLB_STATUS(val)   bfin_write32(ICPLB_STATUS, val)
#define bfin_read_ICPLB_FAULT_ADDR()   bfin_readPTR(ICPLB_FAULT_ADDR)
#define bfin_write_ICPLB_FAULT_ADDR(val) bfin_writePTR(ICPLB_FAULT_ADDR, val)
#define bfin_read_ICPLB_ADDR0()        bfin_readPTR(ICPLB_ADDR0)
#define bfin_write_ICPLB_ADDR0(val)    bfin_writePTR(ICPLB_ADDR0, val)
#define bfin_read_ICPLB_ADDR1()        bfin_readPTR(ICPLB_ADDR1)
#define bfin_write_ICPLB_ADDR1(val)    bfin_writePTR(ICPLB_ADDR1, val)
#define bfin_read_ICPLB_ADDR2()        bfin_readPTR(ICPLB_ADDR2)
#define bfin_write_ICPLB_ADDR2(val)    bfin_writePTR(ICPLB_ADDR2, val)
#define bfin_read_ICPLB_ADDR3()        bfin_readPTR(ICPLB_ADDR3)
#define bfin_write_ICPLB_ADDR3(val)    bfin_writePTR(ICPLB_ADDR3, val)
#define bfin_read_ICPLB_ADDR4()        bfin_readPTR(ICPLB_ADDR4)
#define bfin_write_ICPLB_ADDR4(val)    bfin_writePTR(ICPLB_ADDR4, val)
#define bfin_read_ICPLB_ADDR5()        bfin_readPTR(ICPLB_ADDR5)
#define bfin_write_ICPLB_ADDR5(val)    bfin_writePTR(ICPLB_ADDR5, val)
#define bfin_read_ICPLB_ADDR6()        bfin_readPTR(ICPLB_ADDR6)
#define bfin_write_ICPLB_ADDR6(val)    bfin_writePTR(ICPLB_ADDR6, val)
#define bfin_read_ICPLB_ADDR7()        bfin_readPTR(ICPLB_ADDR7)
#define bfin_write_ICPLB_ADDR7(val)    bfin_writePTR(ICPLB_ADDR7, val)
#define bfin_read_ICPLB_ADDR8()        bfin_readPTR(ICPLB_ADDR8)
#define bfin_write_ICPLB_ADDR8(val)    bfin_writePTR(ICPLB_ADDR8, val)
#define bfin_read_ICPLB_ADDR9()        bfin_readPTR(ICPLB_ADDR9)
#define bfin_write_ICPLB_ADDR9(val)    bfin_writePTR(ICPLB_ADDR9, val)
#define bfin_read_ICPLB_ADDR10()       bfin_readPTR(ICPLB_ADDR10)
#define bfin_write_ICPLB_ADDR10(val)   bfin_writePTR(ICPLB_ADDR10, val)
#define bfin_read_ICPLB_ADDR11()       bfin_readPTR(ICPLB_ADDR11)
#define bfin_write_ICPLB_ADDR11(val)   bfin_writePTR(ICPLB_ADDR11, val)
#define bfin_read_ICPLB_ADDR12()       bfin_readPTR(ICPLB_ADDR12)
#define bfin_write_ICPLB_ADDR12(val)   bfin_writePTR(ICPLB_ADDR12, val)
#define bfin_read_ICPLB_ADDR13()       bfin_readPTR(ICPLB_ADDR13)
#define bfin_write_ICPLB_ADDR13(val)   bfin_writePTR(ICPLB_ADDR13, val)
#define bfin_read_ICPLB_ADDR14()       bfin_readPTR(ICPLB_ADDR14)
#define bfin_write_ICPLB_ADDR14(val)   bfin_writePTR(ICPLB_ADDR14, val)
#define bfin_read_ICPLB_ADDR15()       bfin_readPTR(ICPLB_ADDR15)
#define bfin_write_ICPLB_ADDR15(val)   bfin_writePTR(ICPLB_ADDR15, val)
#define bfin_read_ICPLB_DATA0()        bfin_read32(ICPLB_DATA0)
#define bfin_write_ICPLB_DATA0(val)    bfin_write32(ICPLB_DATA0, val)
#define bfin_read_ICPLB_DATA1()        bfin_read32(ICPLB_DATA1)
#define bfin_write_ICPLB_DATA1(val)    bfin_write32(ICPLB_DATA1, val)
#define bfin_read_ICPLB_DATA2()        bfin_read32(ICPLB_DATA2)
#define bfin_write_ICPLB_DATA2(val)    bfin_write32(ICPLB_DATA2, val)
#define bfin_read_ICPLB_DATA3()        bfin_read32(ICPLB_DATA3)
#define bfin_write_ICPLB_DATA3(val)    bfin_write32(ICPLB_DATA3, val)
#define bfin_read_ICPLB_DATA4()        bfin_read32(ICPLB_DATA4)
#define bfin_write_ICPLB_DATA4(val)    bfin_write32(ICPLB_DATA4, val)
#define bfin_read_ICPLB_DATA5()        bfin_read32(ICPLB_DATA5)
#define bfin_write_ICPLB_DATA5(val)    bfin_write32(ICPLB_DATA5, val)
#define bfin_read_ICPLB_DATA6()        bfin_read32(ICPLB_DATA6)
#define bfin_write_ICPLB_DATA6(val)    bfin_write32(ICPLB_DATA6, val)
#define bfin_read_ICPLB_DATA7()        bfin_read32(ICPLB_DATA7)
#define bfin_write_ICPLB_DATA7(val)    bfin_write32(ICPLB_DATA7, val)
#define bfin_read_ICPLB_DATA8()        bfin_read32(ICPLB_DATA8)
#define bfin_write_ICPLB_DATA8(val)    bfin_write32(ICPLB_DATA8, val)
#define bfin_read_ICPLB_DATA9()        bfin_read32(ICPLB_DATA9)
#define bfin_write_ICPLB_DATA9(val)    bfin_write32(ICPLB_DATA9, val)
#define bfin_read_ICPLB_DATA10()       bfin_read32(ICPLB_DATA10)
#define bfin_write_ICPLB_DATA10(val)   bfin_write32(ICPLB_DATA10, val)
#define bfin_read_ICPLB_DATA11()       bfin_read32(ICPLB_DATA11)
#define bfin_write_ICPLB_DATA11(val)   bfin_write32(ICPLB_DATA11, val)
#define bfin_read_ICPLB_DATA12()       bfin_read32(ICPLB_DATA12)
#define bfin_write_ICPLB_DATA12(val)   bfin_write32(ICPLB_DATA12, val)
#define bfin_read_ICPLB_DATA13()       bfin_read32(ICPLB_DATA13)
#define bfin_write_ICPLB_DATA13(val)   bfin_write32(ICPLB_DATA13, val)
#define bfin_read_ICPLB_DATA14()       bfin_read32(ICPLB_DATA14)
#define bfin_write_ICPLB_DATA14(val)   bfin_write32(ICPLB_DATA14, val)
#define bfin_read_ICPLB_DATA15()       bfin_read32(ICPLB_DATA15)
#define bfin_write_ICPLB_DATA15(val)   bfin_write32(ICPLB_DATA15, val)
#define bfin_read_ITEST_COMMAND()      bfin_read32(ITEST_COMMAND)
#define bfin_write_ITEST_COMMAND(val)  bfin_write32(ITEST_COMMAND, val)
#define bfin_read_ITEST_DATA0()        bfin_read32(ITEST_DATA0)
#define bfin_write_ITEST_DATA0(val)    bfin_write32(ITEST_DATA0, val)
#define bfin_read_ITEST_DATA1()        bfin_read32(ITEST_DATA1)
#define bfin_write_ITEST_DATA1(val)    bfin_write32(ITEST_DATA1, val)
#define bfin_read_EVT0()               bfin_readPTR(EVT0)
#define bfin_write_EVT0(val)           bfin_writePTR(EVT0, val)
#define bfin_read_EVT1()               bfin_readPTR(EVT1)
#define bfin_write_EVT1(val)           bfin_writePTR(EVT1, val)
#define bfin_read_EVT2()               bfin_readPTR(EVT2)
#define bfin_write_EVT2(val)           bfin_writePTR(EVT2, val)
#define bfin_read_EVT3()               bfin_readPTR(EVT3)
#define bfin_write_EVT3(val)           bfin_writePTR(EVT3, val)
#define bfin_read_EVT4()               bfin_readPTR(EVT4)
#define bfin_write_EVT4(val)           bfin_writePTR(EVT4, val)
#define bfin_read_EVT5()               bfin_readPTR(EVT5)
#define bfin_write_EVT5(val)           bfin_writePTR(EVT5, val)
#define bfin_read_EVT6()               bfin_readPTR(EVT6)
#define bfin_write_EVT6(val)           bfin_writePTR(EVT6, val)
#define bfin_read_EVT7()               bfin_readPTR(EVT7)
#define bfin_write_EVT7(val)           bfin_writePTR(EVT7, val)
#define bfin_read_EVT8()               bfin_readPTR(EVT8)
#define bfin_write_EVT8(val)           bfin_writePTR(EVT8, val)
#define bfin_read_EVT9()               bfin_readPTR(EVT9)
#define bfin_write_EVT9(val)           bfin_writePTR(EVT9, val)
#define bfin_read_EVT10()              bfin_readPTR(EVT10)
#define bfin_write_EVT10(val)          bfin_writePTR(EVT10, val)
#define bfin_read_EVT11()              bfin_readPTR(EVT11)
#define bfin_write_EVT11(val)          bfin_writePTR(EVT11, val)
#define bfin_read_EVT12()              bfin_readPTR(EVT12)
#define bfin_write_EVT12(val)          bfin_writePTR(EVT12, val)
#define bfin_read_EVT13()              bfin_readPTR(EVT13)
#define bfin_write_EVT13(val)          bfin_writePTR(EVT13, val)
#define bfin_read_EVT14()              bfin_readPTR(EVT14)
#define bfin_write_EVT14(val)          bfin_writePTR(EVT14, val)
#define bfin_read_EVT15()              bfin_readPTR(EVT15)
#define bfin_write_EVT15(val)          bfin_writePTR(EVT15, val)
#define bfin_read_ILAT()               bfin_read32(ILAT)
#define bfin_write_ILAT(val)           bfin_write32(ILAT, val)
#define bfin_read_IMASK()              bfin_read32(IMASK)
#define bfin_write_IMASK(val)          bfin_write32(IMASK, val)
#define bfin_read_IPEND()              bfin_read32(IPEND)
#define bfin_write_IPEND(val)          bfin_write32(IPEND, val)
#define bfin_read_IPRIO()              bfin_read32(IPRIO)
#define bfin_write_IPRIO(val)          bfin_write32(IPRIO, val)
#define bfin_read_TCNTL()              bfin_read32(TCNTL)
#define bfin_write_TCNTL(val)          bfin_write32(TCNTL, val)
#define bfin_read_TPERIOD()            bfin_read32(TPERIOD)
#define bfin_write_TPERIOD(val)        bfin_write32(TPERIOD, val)
#define bfin_read_TSCALE()             bfin_read32(TSCALE)
#define bfin_write_TSCALE(val)         bfin_write32(TSCALE, val)
#define bfin_read_TCOUNT()             bfin_read32(TCOUNT)
#define bfin_write_TCOUNT(val)         bfin_write32(TCOUNT, val)
#define bfin_read_DSPID()              bfin_read32(DSPID)
#define bfin_write_DSPID(val)          bfin_write32(DSPID, val)
#define bfin_read_RSI_PWR_CONTROL()    bfin_read16(RSI_PWR_CONTROL)
#define bfin_write_RSI_PWR_CONTROL(val) bfin_write16(RSI_PWR_CONTROL, val)
#define bfin_read_RSI_CLK_CONTROL()    bfin_read16(RSI_CLK_CONTROL)
#define bfin_write_RSI_CLK_CONTROL(val) bfin_write16(RSI_CLK_CONTROL, val)
#define bfin_read_RSI_ARGUMENT()       bfin_read32(RSI_ARGUMENT)
#define bfin_write_RSI_ARGUMENT(val)   bfin_write32(RSI_ARGUMENT, val)
#define bfin_read_RSI_COMMAND()        bfin_read16(RSI_COMMAND)
#define bfin_write_RSI_COMMAND(val)    bfin_write16(RSI_COMMAND, val)
#define bfin_read_RSI_RESP_CMD()       bfin_read16(RSI_RESP_CMD)
#define bfin_write_RSI_RESP_CMD(val)   bfin_write16(RSI_RESP_CMD, val)
#define bfin_read_RSI_RESPONSE0()      bfin_read32(RSI_RESPONSE0)
#define bfin_write_RSI_RESPONSE0(val)  bfin_write32(RSI_RESPONSE0, val)
#define bfin_read_RSI_RESPONSE1()      bfin_read32(RSI_RESPONSE1)
#define bfin_write_RSI_RESPONSE1(val)  bfin_write32(RSI_RESPONSE1, val)
#define bfin_read_RSI_RESPONSE2()      bfin_read32(RSI_RESPONSE2)
#define bfin_write_RSI_RESPONSE2(val)  bfin_write32(RSI_RESPONSE2, val)
#define bfin_read_RSI_RESPONSE3()      bfin_read32(RSI_RESPONSE3)
#define bfin_write_RSI_RESPONSE3(val)  bfin_write32(RSI_RESPONSE3, val)
#define bfin_read_RSI_DATA_TIMER()     bfin_read32(RSI_DATA_TIMER)
#define bfin_write_RSI_DATA_TIMER(val) bfin_write32(RSI_DATA_TIMER, val)
#define bfin_read_RSI_DATA_LGTH()      bfin_read16(RSI_DATA_LGTH)
#define bfin_write_RSI_DATA_LGTH(val)  bfin_write16(RSI_DATA_LGTH, val)
#define bfin_read_RSI_DATA_CONTROL()   bfin_read16(RSI_DATA_CONTROL)
#define bfin_write_RSI_DATA_CONTROL(val) bfin_write16(RSI_DATA_CONTROL, val)
#define bfin_read_RSI_DATA_CNT()       bfin_read16(RSI_DATA_CNT)
#define bfin_write_RSI_DATA_CNT(val)   bfin_write16(RSI_DATA_CNT, val)
#define bfin_read_RSI_STATUS()         bfin_read32(RSI_STATUS)
#define bfin_write_RSI_STATUS(val)     bfin_write32(RSI_STATUS, val)
#define bfin_read_RSI_STATUSCL()       bfin_read16(RSI_STATUSCL)
#define bfin_write_RSI_STATUSCL(val)   bfin_write16(RSI_STATUSCL, val)
#define bfin_read_RSI_MASK0()          bfin_read32(RSI_MASK0)
#define bfin_write_RSI_MASK0(val)      bfin_write32(RSI_MASK0, val)
#define bfin_read_RSI_MASK1()          bfin_read32(RSI_MASK1)
#define bfin_write_RSI_MASK1(val)      bfin_write32(RSI_MASK1, val)
#define bfin_read_RSI_FIFO_CNT()       bfin_read16(RSI_FIFO_CNT)
#define bfin_write_RSI_FIFO_CNT(val)   bfin_write16(RSI_FIFO_CNT, val)
#define bfin_read_RSI_CEATA_CONTROL()  bfin_read16(RSI_CEATA_CONTROL)
#define bfin_write_RSI_CEATA_CONTROL(val) bfin_write16(RSI_CEATA_CONTROL, val)
#define bfin_read_RSI_FIFO()           bfin_read32(RSI_FIFO)
#define bfin_write_RSI_FIFO(val)       bfin_write32(RSI_FIFO, val)
#define bfin_read_RSI_ESTAT()          bfin_read16(RSI_ESTAT)
#define bfin_write_RSI_ESTAT(val)      bfin_write16(RSI_ESTAT, val)
#define bfin_read_RSI_EMASK()          bfin_read16(RSI_EMASK)
#define bfin_write_RSI_EMASK(val)      bfin_write16(RSI_EMASK, val)
#define bfin_read_RSI_CONFIG()         bfin_read16(RSI_CONFIG)
#define bfin_write_RSI_CONFIG(val)     bfin_write16(RSI_CONFIG, val)
#define bfin_read_RSI_RD_WAIT_EN()     bfin_read16(RSI_RD_WAIT_EN)
#define bfin_write_RSI_RD_WAIT_EN(val) bfin_write16(RSI_RD_WAIT_EN, val)
#define bfin_read_RSI_PID0()           bfin_read16(RSI_PID0)
#define bfin_write_RSI_PID0(val)       bfin_write16(RSI_PID0, val)
#define bfin_read_RSI_PID1()           bfin_read16(RSI_PID1)
#define bfin_write_RSI_PID1(val)       bfin_write16(RSI_PID1, val)
#define bfin_read_RSI_PID2()           bfin_read16(RSI_PID2)
#define bfin_write_RSI_PID2(val)       bfin_write16(RSI_PID2, val)
#define bfin_read_RSI_PID3()           bfin_read16(RSI_PID3)
#define bfin_write_RSI_PID3(val)       bfin_write16(RSI_PID3, val)
#define bfin_read_EMAC_OPMODE()        bfin_read32(EMAC_OPMODE)
#define bfin_write_EMAC_OPMODE(val)    bfin_write32(EMAC_OPMODE, val)
#define bfin_read_EMAC_ADDRLO()        bfin_read32(EMAC_ADDRLO)
#define bfin_write_EMAC_ADDRLO(val)    bfin_write32(EMAC_ADDRLO, val)
#define bfin_read_EMAC_ADDRHI()        bfin_read32(EMAC_ADDRHI)
#define bfin_write_EMAC_ADDRHI(val)    bfin_write32(EMAC_ADDRHI, val)
#define bfin_read_EMAC_HASHLO()        bfin_read32(EMAC_HASHLO)
#define bfin_write_EMAC_HASHLO(val)    bfin_write32(EMAC_HASHLO, val)
#define bfin_read_EMAC_HASHHI()        bfin_read32(EMAC_HASHHI)
#define bfin_write_EMAC_HASHHI(val)    bfin_write32(EMAC_HASHHI, val)
#define bfin_read_EMAC_STAADD()        bfin_read32(EMAC_STAADD)
#define bfin_write_EMAC_STAADD(val)    bfin_write32(EMAC_STAADD, val)
#define bfin_read_EMAC_STADAT()        bfin_read32(EMAC_STADAT)
#define bfin_write_EMAC_STADAT(val)    bfin_write32(EMAC_STADAT, val)
#define bfin_read_EMAC_FLC()           bfin_read32(EMAC_FLC)
#define bfin_write_EMAC_FLC(val)       bfin_write32(EMAC_FLC, val)
#define bfin_read_EMAC_VLAN1()         bfin_read32(EMAC_VLAN1)
#define bfin_write_EMAC_VLAN1(val)     bfin_write32(EMAC_VLAN1, val)
#define bfin_read_EMAC_VLAN2()         bfin_read32(EMAC_VLAN2)
#define bfin_write_EMAC_VLAN2(val)     bfin_write32(EMAC_VLAN2, val)
#define bfin_read_EMAC_WKUP_CTL()      bfin_read32(EMAC_WKUP_CTL)
#define bfin_write_EMAC_WKUP_CTL(val)  bfin_write32(EMAC_WKUP_CTL, val)
#define bfin_read_EMAC_WKUP_FFMSK0()   bfin_read32(EMAC_WKUP_FFMSK0)
#define bfin_write_EMAC_WKUP_FFMSK0(val) bfin_write32(EMAC_WKUP_FFMSK0, val)
#define bfin_read_EMAC_WKUP_FFMSK1()   bfin_read32(EMAC_WKUP_FFMSK1)
#define bfin_write_EMAC_WKUP_FFMSK1(val) bfin_write32(EMAC_WKUP_FFMSK1, val)
#define bfin_read_EMAC_WKUP_FFMSK2()   bfin_read32(EMAC_WKUP_FFMSK2)
#define bfin_write_EMAC_WKUP_FFMSK2(val) bfin_write32(EMAC_WKUP_FFMSK2, val)
#define bfin_read_EMAC_WKUP_FFMSK3()   bfin_read32(EMAC_WKUP_FFMSK3)
#define bfin_write_EMAC_WKUP_FFMSK3(val) bfin_write32(EMAC_WKUP_FFMSK3, val)
#define bfin_read_EMAC_WKUP_FFCMD()    bfin_read32(EMAC_WKUP_FFCMD)
#define bfin_write_EMAC_WKUP_FFCMD(val) bfin_write32(EMAC_WKUP_FFCMD, val)
#define bfin_read_EMAC_WKUP_FFOFF()    bfin_read32(EMAC_WKUP_FFOFF)
#define bfin_write_EMAC_WKUP_FFOFF(val) bfin_write32(EMAC_WKUP_FFOFF, val)
#define bfin_read_EMAC_WKUP_FFCRC0()   bfin_read32(EMAC_WKUP_FFCRC0)
#define bfin_write_EMAC_WKUP_FFCRC0(val) bfin_write32(EMAC_WKUP_FFCRC0, val)
#define bfin_read_EMAC_WKUP_FFCRC1()   bfin_read32(EMAC_WKUP_FFCRC1)
#define bfin_write_EMAC_WKUP_FFCRC1(val) bfin_write32(EMAC_WKUP_FFCRC1, val)
#define bfin_read_EMAC_SYSCTL()        bfin_read32(EMAC_SYSCTL)
#define bfin_write_EMAC_SYSCTL(val)    bfin_write32(EMAC_SYSCTL, val)
#define bfin_read_EMAC_SYSTAT()        bfin_read32(EMAC_SYSTAT)
#define bfin_write_EMAC_SYSTAT(val)    bfin_write32(EMAC_SYSTAT, val)
#define bfin_read_EMAC_RX_STAT()       bfin_read32(EMAC_RX_STAT)
#define bfin_write_EMAC_RX_STAT(val)   bfin_write32(EMAC_RX_STAT, val)
#define bfin_read_EMAC_RX_STKY()       bfin_read32(EMAC_RX_STKY)
#define bfin_write_EMAC_RX_STKY(val)   bfin_write32(EMAC_RX_STKY, val)
#define bfin_read_EMAC_RX_IRQE()       bfin_read32(EMAC_RX_IRQE)
#define bfin_write_EMAC_RX_IRQE(val)   bfin_write32(EMAC_RX_IRQE, val)
#define bfin_read_EMAC_TX_STAT()       bfin_read32(EMAC_TX_STAT)
#define bfin_write_EMAC_TX_STAT(val)   bfin_write32(EMAC_TX_STAT, val)
#define bfin_read_EMAC_TX_STKY()       bfin_read32(EMAC_TX_STKY)
#define bfin_write_EMAC_TX_STKY(val)   bfin_write32(EMAC_TX_STKY, val)
#define bfin_read_EMAC_TX_IRQE()       bfin_read32(EMAC_TX_IRQE)
#define bfin_write_EMAC_TX_IRQE(val)   bfin_write32(EMAC_TX_IRQE, val)
#define bfin_read_EMAC_MMC_CTL()       bfin_read32(EMAC_MMC_CTL)
#define bfin_write_EMAC_MMC_CTL(val)   bfin_write32(EMAC_MMC_CTL, val)
#define bfin_read_EMAC_MMC_RIRQS()     bfin_read32(EMAC_MMC_RIRQS)
#define bfin_write_EMAC_MMC_RIRQS(val) bfin_write32(EMAC_MMC_RIRQS, val)
#define bfin_read_EMAC_MMC_RIRQE()     bfin_read32(EMAC_MMC_RIRQE)
#define bfin_write_EMAC_MMC_RIRQE(val) bfin_write32(EMAC_MMC_RIRQE, val)
#define bfin_read_EMAC_MMC_TIRQS()     bfin_read32(EMAC_MMC_TIRQS)
#define bfin_write_EMAC_MMC_TIRQS(val) bfin_write32(EMAC_MMC_TIRQS, val)
#define bfin_read_EMAC_MMC_TIRQE()     bfin_read32(EMAC_MMC_TIRQE)
#define bfin_write_EMAC_MMC_TIRQE(val) bfin_write32(EMAC_MMC_TIRQE, val)
#define bfin_read_EMAC_RXC_OK()        bfin_read32(EMAC_RXC_OK)
#define bfin_write_EMAC_RXC_OK(val)    bfin_write32(EMAC_RXC_OK, val)
#define bfin_read_EMAC_RXC_FCS()       bfin_read32(EMAC_RXC_FCS)
#define bfin_write_EMAC_RXC_FCS(val)   bfin_write32(EMAC_RXC_FCS, val)
#define bfin_read_EMAC_RXC_ALIGN()     bfin_read32(EMAC_RXC_ALIGN)
#define bfin_write_EMAC_RXC_ALIGN(val) bfin_write32(EMAC_RXC_ALIGN, val)
#define bfin_read_EMAC_RXC_OCTET()     bfin_read32(EMAC_RXC_OCTET)
#define bfin_write_EMAC_RXC_OCTET(val) bfin_write32(EMAC_RXC_OCTET, val)
#define bfin_read_EMAC_RXC_DMAOVF()    bfin_read32(EMAC_RXC_DMAOVF)
#define bfin_write_EMAC_RXC_DMAOVF(val) bfin_write32(EMAC_RXC_DMAOVF, val)
#define bfin_read_EMAC_RXC_UNICST()    bfin_read32(EMAC_RXC_UNICST)
#define bfin_write_EMAC_RXC_UNICST(val) bfin_write32(EMAC_RXC_UNICST, val)
#define bfin_read_EMAC_RXC_MULTI()     bfin_read32(EMAC_RXC_MULTI)
#define bfin_write_EMAC_RXC_MULTI(val) bfin_write32(EMAC_RXC_MULTI, val)
#define bfin_read_EMAC_RXC_BROAD()     bfin_read32(EMAC_RXC_BROAD)
#define bfin_write_EMAC_RXC_BROAD(val) bfin_write32(EMAC_RXC_BROAD, val)
#define bfin_read_EMAC_RXC_LNERRI()    bfin_read32(EMAC_RXC_LNERRI)
#define bfin_write_EMAC_RXC_LNERRI(val) bfin_write32(EMAC_RXC_LNERRI, val)
#define bfin_read_EMAC_RXC_LNERRO()    bfin_read32(EMAC_RXC_LNERRO)
#define bfin_write_EMAC_RXC_LNERRO(val) bfin_write32(EMAC_RXC_LNERRO, val)
#define bfin_read_EMAC_RXC_LONG()      bfin_read32(EMAC_RXC_LONG)
#define bfin_write_EMAC_RXC_LONG(val)  bfin_write32(EMAC_RXC_LONG, val)
#define bfin_read_EMAC_RXC_MACCTL()    bfin_read32(EMAC_RXC_MACCTL)
#define bfin_write_EMAC_RXC_MACCTL(val) bfin_write32(EMAC_RXC_MACCTL, val)
#define bfin_read_EMAC_RXC_OPCODE()    bfin_read32(EMAC_RXC_OPCODE)
#define bfin_write_EMAC_RXC_OPCODE(val) bfin_write32(EMAC_RXC_OPCODE, val)
#define bfin_read_EMAC_RXC_PAUSE()     bfin_read32(EMAC_RXC_PAUSE)
#define bfin_write_EMAC_RXC_PAUSE(val) bfin_write32(EMAC_RXC_PAUSE, val)
#define bfin_read_EMAC_RXC_ALLFRM()    bfin_read32(EMAC_RXC_ALLFRM)
#define bfin_write_EMAC_RXC_ALLFRM(val) bfin_write32(EMAC_RXC_ALLFRM, val)
#define bfin_read_EMAC_RXC_ALLOCT()    bfin_read32(EMAC_RXC_ALLOCT)
#define bfin_write_EMAC_RXC_ALLOCT(val) bfin_write32(EMAC_RXC_ALLOCT, val)
#define bfin_read_EMAC_RXC_TYPED()     bfin_read32(EMAC_RXC_TYPED)
#define bfin_write_EMAC_RXC_TYPED(val) bfin_write32(EMAC_RXC_TYPED, val)
#define bfin_read_EMAC_RXC_SHORT()     bfin_read32(EMAC_RXC_SHORT)
#define bfin_write_EMAC_RXC_SHORT(val) bfin_write32(EMAC_RXC_SHORT, val)
#define bfin_read_EMAC_RXC_EQ64()      bfin_read32(EMAC_RXC_EQ64)
#define bfin_write_EMAC_RXC_EQ64(val)  bfin_write32(EMAC_RXC_EQ64, val)
#define bfin_read_EMAC_RXC_LT128()     bfin_read32(EMAC_RXC_LT128)
#define bfin_write_EMAC_RXC_LT128(val) bfin_write32(EMAC_RXC_LT128, val)
#define bfin_read_EMAC_RXC_LT256()     bfin_read32(EMAC_RXC_LT256)
#define bfin_write_EMAC_RXC_LT256(val) bfin_write32(EMAC_RXC_LT256, val)
#define bfin_read_EMAC_RXC_LT512()     bfin_read32(EMAC_RXC_LT512)
#define bfin_write_EMAC_RXC_LT512(val) bfin_write32(EMAC_RXC_LT512, val)
#define bfin_read_EMAC_RXC_LT1024()    bfin_read32(EMAC_RXC_LT1024)
#define bfin_write_EMAC_RXC_LT1024(val) bfin_write32(EMAC_RXC_LT1024, val)
#define bfin_read_EMAC_RXC_GE1024()    bfin_read32(EMAC_RXC_GE1024)
#define bfin_write_EMAC_RXC_GE1024(val) bfin_write32(EMAC_RXC_GE1024, val)
#define bfin_read_EMAC_TXC_OK()        bfin_read32(EMAC_TXC_OK)
#define bfin_write_EMAC_TXC_OK(val)    bfin_write32(EMAC_TXC_OK, val)
#define bfin_read_EMAC_TXC_1COL()      bfin_read32(EMAC_TXC_1COL)
#define bfin_write_EMAC_TXC_1COL(val)  bfin_write32(EMAC_TXC_1COL, val)
#define bfin_read_EMAC_TXC_GT1COL()    bfin_read32(EMAC_TXC_GT1COL)
#define bfin_write_EMAC_TXC_GT1COL(val) bfin_write32(EMAC_TXC_GT1COL, val)
#define bfin_read_EMAC_TXC_OCTET()     bfin_read32(EMAC_TXC_OCTET)
#define bfin_write_EMAC_TXC_OCTET(val) bfin_write32(EMAC_TXC_OCTET, val)
#define bfin_read_EMAC_TXC_DEFER()     bfin_read32(EMAC_TXC_DEFER)
#define bfin_write_EMAC_TXC_DEFER(val) bfin_write32(EMAC_TXC_DEFER, val)
#define bfin_read_EMAC_TXC_LATECL()    bfin_read32(EMAC_TXC_LATECL)
#define bfin_write_EMAC_TXC_LATECL(val) bfin_write32(EMAC_TXC_LATECL, val)
#define bfin_read_EMAC_TXC_XS_COL()    bfin_read32(EMAC_TXC_XS_COL)
#define bfin_write_EMAC_TXC_XS_COL(val) bfin_write32(EMAC_TXC_XS_COL, val)
#define bfin_read_EMAC_TXC_DMAUND()    bfin_read32(EMAC_TXC_DMAUND)
#define bfin_write_EMAC_TXC_DMAUND(val) bfin_write32(EMAC_TXC_DMAUND, val)
#define bfin_read_EMAC_TXC_CRSERR()    bfin_read32(EMAC_TXC_CRSERR)
#define bfin_write_EMAC_TXC_CRSERR(val) bfin_write32(EMAC_TXC_CRSERR, val)
#define bfin_read_EMAC_TXC_UNICST()    bfin_read32(EMAC_TXC_UNICST)
#define bfin_write_EMAC_TXC_UNICST(val) bfin_write32(EMAC_TXC_UNICST, val)
#define bfin_read_EMAC_TXC_MULTI()     bfin_read32(EMAC_TXC_MULTI)
#define bfin_write_EMAC_TXC_MULTI(val) bfin_write32(EMAC_TXC_MULTI, val)
#define bfin_read_EMAC_TXC_BROAD()     bfin_read32(EMAC_TXC_BROAD)
#define bfin_write_EMAC_TXC_BROAD(val) bfin_write32(EMAC_TXC_BROAD, val)
#define bfin_read_EMAC_TXC_XS_DFR()    bfin_read32(EMAC_TXC_XS_DFR)
#define bfin_write_EMAC_TXC_XS_DFR(val) bfin_write32(EMAC_TXC_XS_DFR, val)
#define bfin_read_EMAC_TXC_MACCTL()    bfin_read32(EMAC_TXC_MACCTL)
#define bfin_write_EMAC_TXC_MACCTL(val) bfin_write32(EMAC_TXC_MACCTL, val)
#define bfin_read_EMAC_TXC_ALLFRM()    bfin_read32(EMAC_TXC_ALLFRM)
#define bfin_write_EMAC_TXC_ALLFRM(val) bfin_write32(EMAC_TXC_ALLFRM, val)
#define bfin_read_EMAC_TXC_ALLOCT()    bfin_read32(EMAC_TXC_ALLOCT)
#define bfin_write_EMAC_TXC_ALLOCT(val) bfin_write32(EMAC_TXC_ALLOCT, val)
#define bfin_read_EMAC_TXC_EQ64()      bfin_read32(EMAC_TXC_EQ64)
#define bfin_write_EMAC_TXC_EQ64(val)  bfin_write32(EMAC_TXC_EQ64, val)
#define bfin_read_EMAC_TXC_LT128()     bfin_read32(EMAC_TXC_LT128)
#define bfin_write_EMAC_TXC_LT128(val) bfin_write32(EMAC_TXC_LT128, val)
#define bfin_read_EMAC_TXC_LT256()     bfin_read32(EMAC_TXC_LT256)
#define bfin_write_EMAC_TXC_LT256(val) bfin_write32(EMAC_TXC_LT256, val)
#define bfin_read_EMAC_TXC_LT512()     bfin_read32(EMAC_TXC_LT512)
#define bfin_write_EMAC_TXC_LT512(val) bfin_write32(EMAC_TXC_LT512, val)
#define bfin_read_EMAC_TXC_LT1024()    bfin_read32(EMAC_TXC_LT1024)
#define bfin_write_EMAC_TXC_LT1024(val) bfin_write32(EMAC_TXC_LT1024, val)
#define bfin_read_EMAC_TXC_GE1024()    bfin_read32(EMAC_TXC_GE1024)
#define bfin_write_EMAC_TXC_GE1024(val) bfin_write32(EMAC_TXC_GE1024, val)
#define bfin_read_EMAC_TXC_ABORT()     bfin_read32(EMAC_TXC_ABORT)
#define bfin_write_EMAC_TXC_ABORT(val) bfin_write32(EMAC_TXC_ABORT, val)
#define bfin_read_EMAC_PTP_CTL()       bfin_read16(EMAC_PTP_CTL)
#define bfin_write_EMAC_PTP_CTL(val)   bfin_write16(EMAC_PTP_CTL, val)
#define bfin_read_EMAC_PTP_IE()        bfin_read16(EMAC_PTP_IE)
#define bfin_write_EMAC_PTP_IE(val)    bfin_write16(EMAC_PTP_IE, val)
#define bfin_read_EMAC_PTP_ISTAT()     bfin_read16(EMAC_PTP_ISTAT)
#define bfin_write_EMAC_PTP_ISTAT(val) bfin_write16(EMAC_PTP_ISTAT, val)
#define bfin_read_EMAC_PTP_FOFF()      bfin_read32(EMAC_PTP_FOFF)
#define bfin_write_EMAC_PTP_FOFF(val)  bfin_write32(EMAC_PTP_FOFF, val)
#define bfin_read_EMAC_PTP_FV1()       bfin_read32(EMAC_PTP_FV1)
#define bfin_write_EMAC_PTP_FV1(val)   bfin_write32(EMAC_PTP_FV1, val)
#define bfin_read_EMAC_PTP_FV2()       bfin_read32(EMAC_PTP_FV2)
#define bfin_write_EMAC_PTP_FV2(val)   bfin_write32(EMAC_PTP_FV2, val)
#define bfin_read_EMAC_PTP_FV3()       bfin_read32(EMAC_PTP_FV3)
#define bfin_write_EMAC_PTP_FV3(val)   bfin_write32(EMAC_PTP_FV3, val)
#define bfin_read_EMAC_PTP_ADDEND()    bfin_read32(EMAC_PTP_ADDEND)
#define bfin_write_EMAC_PTP_ADDEND(val) bfin_write32(EMAC_PTP_ADDEND, val)
#define bfin_read_EMAC_PTP_ACCR()      bfin_read32(EMAC_PTP_ACCR)
#define bfin_write_EMAC_PTP_ACCR(val)  bfin_write32(EMAC_PTP_ACCR, val)
#define bfin_read_EMAC_PTP_OFFSET()    bfin_read32(EMAC_PTP_OFFSET)
#define bfin_write_EMAC_PTP_OFFSET(val) bfin_write32(EMAC_PTP_OFFSET, val)
#define bfin_read_EMAC_PTP_TIMELO()    bfin_read32(EMAC_PTP_TIMELO)
#define bfin_write_EMAC_PTP_TIMELO(val) bfin_write32(EMAC_PTP_TIMELO, val)
#define bfin_read_EMAC_PTP_TIMEHI()    bfin_read32(EMAC_PTP_TIMEHI)
#define bfin_write_EMAC_PTP_TIMEHI(val) bfin_write32(EMAC_PTP_TIMEHI, val)
#define bfin_read_EMAC_PTP_RXSNAPLO()  bfin_read32(EMAC_PTP_RXSNAPLO)
#define bfin_write_EMAC_PTP_RXSNAPLO(val) bfin_write32(EMAC_PTP_RXSNAPLO, val)
#define bfin_read_EMAC_PTP_RXSNAPHI()  bfin_read32(EMAC_PTP_RXSNAPHI)
#define bfin_write_EMAC_PTP_RXSNAPHI(val) bfin_write32(EMAC_PTP_RXSNAPHI, val)
#define bfin_read_EMAC_PTP_TXSNAPLO()  bfin_read32(EMAC_PTP_TXSNAPLO)
#define bfin_write_EMAC_PTP_TXSNAPLO(val) bfin_write32(EMAC_PTP_TXSNAPLO, val)
#define bfin_read_EMAC_PTP_TXSNAPHI()  bfin_read32(EMAC_PTP_TXSNAPHI)
#define bfin_write_EMAC_PTP_TXSNAPHI(val) bfin_write32(EMAC_PTP_TXSNAPHI, val)
#define bfin_read_EMAC_PTP_ALARMLO()   bfin_read32(EMAC_PTP_ALARMLO)
#define bfin_write_EMAC_PTP_ALARMLO(val) bfin_write32(EMAC_PTP_ALARMLO, val)
#define bfin_read_EMAC_PTP_ALARMHI()   bfin_read32(EMAC_PTP_ALARMHI)
#define bfin_write_EMAC_PTP_ALARMHI(val) bfin_write32(EMAC_PTP_ALARMHI, val)
#define bfin_read_EMAC_PTP_ID_OFF()    bfin_read16(EMAC_PTP_ID_OFF)
#define bfin_write_EMAC_PTP_ID_OFF(val) bfin_write16(EMAC_PTP_ID_OFF, val)
#define bfin_read_EMAC_PTP_ID_SNAP()   bfin_read32(EMAC_PTP_ID_SNAP)
#define bfin_write_EMAC_PTP_ID_SNAP(val) bfin_write32(EMAC_PTP_ID_SNAP, val)
#define bfin_read_EMAC_PTP_PPS_STARTLO() bfin_read32(EMAC_PTP_PPS_STARTLO)
#define bfin_write_EMAC_PTP_PPS_STARTLO(val) bfin_write32(EMAC_PTP_PPS_STARTLO, val)
#define bfin_read_EMAC_PTP_PPS_STARTHI() bfin_read32(EMAC_PTP_PPS_STARTHI)
#define bfin_write_EMAC_PTP_PPS_STARTHI(val) bfin_write32(EMAC_PTP_PPS_STARTHI, val)
#define bfin_read_EMAC_PTP_PPS_PERIOD() bfin_read32(EMAC_PTP_PPS_PERIOD)
#define bfin_write_EMAC_PTP_PPS_PERIOD(val) bfin_write32(EMAC_PTP_PPS_PERIOD, val)

#endif /* __BFIN_CDEF_ADSP_BF518_proc__ */
