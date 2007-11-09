/*
 * OTP Masks
 */

#ifndef __BFIN_PERIPHERAL_OTP__
#define __BFIN_PERIPHERAL_OTP__

#ifndef __ASSEMBLY__

uint32_t (* const otp_command)(uint32_t command, uint32_t value) = (void *)_BOOTROM_OTP_COMMAND;
uint32_t (* const otp_read)(uint32_t page, uint32_t flags, uint64_t *page_content) = (void *)_BOOTROM_OTP_READ;
uint32_t (* const otp_write)(uint32_t page, uint32_t flags, uint64_t *page_content) = (void *)_BOOTROM_OTP_WRITE;

#endif

/* Commands for command function */
#define OTP_INIT             0x01
#define OTP_CLOSE            0x02

/* Flags for read/write functions */
#define OTP_LOWER_HALF       0x00
#define OTP_UPPER_HALF       0x01
#define OTP_NO_ECC           0x10
#define OTP_LOCK             0x20
#define OTP_ACCESS_READ      0x1000
#define OTP_ACCESS_READWRITE 0x2000

/* Return values for read/write functions */
#define OTP_MASTER_ERROR     0x001
#define OTP_WRITE_ERROR      0x003
#define OTP_READ_ERROR       0x005
#define OTP_ACC_VIO_ERROR    0x009
#define OTP_DATA_MULT_ERROR  0x011
#define OTP_ECC_MULT_ERROR   0x021
#define OTP_PREV_WR_ERROR    0x041
#define OTP_DATA_SB_WARN     0x100
#define OTP_ECC_SB_WARN      0x200

#endif
