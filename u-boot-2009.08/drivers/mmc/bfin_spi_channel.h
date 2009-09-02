/************************************************************
*
* Copyright (C) 2003, Motorola. All Rights Reserved
*
* FILE spi.h
* PROGRAMMER(S): J.X.Chang (jxchang@motorola.com)
*
*
* DATE OF CREATION: March 8, 2003
*
* SYNOPSIS:
*
* DESCRIPTION: It's driver of SPI in ADSP25535(ADI's DSP). It can
*              only be used in unix or linux.
* CAUTION:     User should use 'ioctl' to change it's
*               configuration just after openning device.
**************************************************************
* MODIFICATION HISTORY:
* March 8, 2003   File spi.h Created.
************************************************************/

#ifndef _SPI_CHANNEL_H_
#define _SPI_CHANNEL_H_

#define SPI0_REGBASE       0xffc00500

struct spi_device_t {
	char  *dev_name;

	unsigned short     flag;
	unsigned short     bdrate;
	unsigned short     enable;
	unsigned short     master;
	unsigned short     out_opendrain;
	unsigned short     polar;
	unsigned short     phase;
	unsigned short     byteorder;/* 0: MSB first; 1: LSB first; */
	unsigned short     size;     /* 0: 8 bits; 1: 16 bits */
	unsigned short     emiso;
	unsigned short     send_zero;
	unsigned short     more_data;
	unsigned short     slave_sel;
	unsigned short     ti_mod;

	void               *priv_data;
};

#define SPI_CTRL_OFF            0x0
#define SPI_FLAG_OFF            0x4
#define SPI_STAT_OFF            0x8
#define SPI_TXBUFF_OFF          0xc
#define SPI_RXBUFF_OFF          0x10
#define SPI_BAUD_OFF            0x14
#define SPI_SHAW_OFF            0x18

#endif /* _SPI_CHANNEL_H_ */
