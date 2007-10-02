/* These addresses are extracted from reading the jump table
 * in the Blackfin on-chip bootrom firmware.
 */

#define _BOOTROM_RESET 0xEF000000
#define _BOOTROM_FINAL_INIT 0xEF000002
/* reserved 0xEF000004 */
#define _BOOTROM_DO_MEMORY_DMA 0xEF000006
#define _BOOTROM_BOOT_DXE_FLASH 0xEF000008
#define _BOOTROM_BOOT_DXE_SPI 0xEF00000A
#define _BOOTROM_BOOT_DXE_TWI 0xEF00000C
/* reserved 0xEF00000E */
#define _BOOTROM_GET_DXE_ADDRESS_FLASH 0xEF000010
#define _BOOTROM_GET_DXE_ADDRESS_SPI 0xEF000012
#define _BOOTROM_GET_DXE_ADDRESS_TWI 0xEF000014
/* reserved 0xEF000014 */
/* reserved 0xEF000016 */
