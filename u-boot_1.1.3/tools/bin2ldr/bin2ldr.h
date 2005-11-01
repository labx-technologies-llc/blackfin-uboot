/*
 * Blackfin can support several boot modes
 */
#define BF537_UART_BOOT		0x0001	/* Uboot boot from UART, for BF537 only */
#define BF5XX_SPI_BOOT		0x0002	/* Uboot boot from SPI flash		*/
#define BF5XX_ASYNC_BOOT	0x0004	/* Uboot boot from externel memory      */

/* Define where the uboot will be loaded by on-chip boot rom */
#define APP_ENTRY 0x00001000

/* Define the boot mode */
#define BFIN_BOOT_MODE		BF5XX_SPI_BOOT
