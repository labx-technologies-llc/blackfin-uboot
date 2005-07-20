#include <common.h>
#include <linux/ctype.h>

#if defined(CONFIG_SPI)

//Application definitions
#define COMMON_SPI_SETTINGS (SPE|MSTR|CPHA|CPOL) //Settings to the SPI_CTL
#define TIMOD01 (0x01)		//stes the SPI to work with core instructions
#define BAUD_RATE_DIVISOR 133

//Flash commands
#define SPI_WREN            (0x06)  //Set Write Enable Latch
#define SPI_WRDI            (0x04)  //Reset Write Enable Latch
#define SPI_RDSR            (0x05)  //Read Status Register
#define SPI_WRSR            (0x01)  //Write Status Register
#define SPI_READ            (0x03)  //Read data from memory
#define SPI_PP              (0x02)  //Program Data into memory
#define SPI_SE              (0xD8)  //Erase one sector in memory
#define SPI_BE              (0xC7)  //Erase all memory
#define WIP					(0x1)	//Check the write in progress bit of the SPI status register
#define WEL					(0x2)	//Check the write enable bit of the SPI status register

#define TIMEOUT 35000

typedef enum
{
	NO_ERR,
	POLL_TIMEOUT,
}ERROR_CODE;
	

void spi_init_f (void);
void spi_init_r (void);
ssize_t spi_read (uchar *, int, uchar *, int);
ssize_t spi_write (uchar *, int, uchar *, int);

char ReadStatusRegister(void);
void Wait_For_SPIF(void);
void SetupSPI( const int spi_setting );
void SPI_OFF(void);
void SendSingleCommand( const int iCommand );

ERROR_CODE ReadData(  unsigned long ulStart, long lCount,char *pnData  );
ERROR_CODE WriteData ( unsigned long ulStartAddr, long lTransferCount, char *iDataSource );
ERROR_CODE Wait_For_Status( char Statusbit );
ERROR_CODE Wait_For_WEL(void);

/* -------------------
 * Variables
 * ------------------- */

/* **************************************************************************
 *
 *  Function:    spi_init_f
 *
 *  Description: Init SPI-Controller (ROM part)
 *
 *  return:      ---
 *
 * *********************************************************************** */
void spi_init_f (void)
{
}

/* **************************************************************************
 *
 *  Function:    spi_init_r
 *
 *  Description: Init SPI-Controller (RAM part) -
 *		 The malloc engine is ready and we can move our buffers to
 *		 normal RAM
 *
 *  return:      ---
 *
 * *********************************************************************** */
void spi_init_r (void)
{
	return;
}

/****************************************************************************
 *  Function:    spi_write
 **************************************************************************** */
ssize_t spi_write (uchar *addr, int alen, uchar *buffer, int len)
{
	unsigned long offset;
	offset = addr[0]<<16 | addr[1] <<8 | addr[2];
	WriteData(offset, len, (char *)buffer);
	return len;
}

/****************************************************************************
 *  Function:    spi_read
 **************************************************************************** */
ssize_t spi_read (uchar *addr, int alen, uchar *buffer, int len)
{
	unsigned long offset;
	offset = addr[0]<<16 | addr[1] <<8 | addr[2];
	ReadData ( offset, len, (char *)buffer );
	return len;
}

void SendSingleCommand( const int iCommand )
{
	int n;

	/*turns on the SPI in single write mode*/
	SetupSPI( (COMMON_SPI_SETTINGS|TIMOD01) );

	/*sends the actual command to the SPI TX register*/
	*pSPI_TDBR = iCommand;

	/*The SPI status register will be polled to check the SPIF bit*/
	Wait_For_SPIF();

	/*The SPI will be turned off*/
	SPI_OFF();

	
	/*Pause before continuing*/
	for(n=0; n<1000; n++)
	{
		asm("nop;");
	}
}

void SetupSPI( const int spi_setting )
{
	
	/*sets up the PF2 to be the slave select of the SPI */
	*pSPI_FLG = 0xFB04;
	*pSPI_BAUD = BAUD_RATE_DIVISOR;
	*pSPI_CTL = spi_setting;

}

void SPI_OFF(void)
{
	int i;
	
	*pSPI_CTL = 0x0400;	/* disable SPI*/
	*pSPI_FLG = 0;
	*pSPI_BAUD = 0;
	
	for(i=0; i<1000; i++)
	{
		asm("nop;");
	}


}

void Wait_For_SPIF(void)
{
	int n;
	unsigned short dummyread;
	
	for(n=0; n<1000; n++)
	{
		asm("nop;");
	}
	
	while(1)
	{
		unsigned short iTest = *pSPI_STAT;
		if( (iTest & SPIF) )
		{
			break;
		}
	}
	dummyread = *pSPI_RDBR; /* read dummy to empty the receive register*/
}

ERROR_CODE Wait_For_WEL(void)
{
	int n, i;
	char status_register = 0;
	ERROR_CODE ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash*/
	
		for(i = 0; i < TIMEOUT; i++)
		{
			status_register = ReadStatusRegister();
			if( (status_register & WEL) )
			{
				ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash*/
				break;
			}
			
			for(n=0; n<1000; n++)
				asm("nop;");
			ErrorCode = POLL_TIMEOUT;	/* Time out error*/
		};
		
	
	return ErrorCode;
}

ERROR_CODE Wait_For_Status( char Statusbit )
{
	int n, i;
	char status_register = 0xFF;
	ERROR_CODE ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash */
	
		for(i = 0; i < TIMEOUT; i++)
		{
			status_register = ReadStatusRegister();
			if( !(status_register & Statusbit) )
			{
				ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash */
				break;
			}
			
			for(n=0; n<1000; n++)
				asm("nop;");
			ErrorCode = POLL_TIMEOUT;	/* Time out error */
		};
		
	
	return ErrorCode;
	
}


char ReadStatusRegister(void)
{
	char dummyread, status_register = 0;
	int n;

	SetupSPI( (COMMON_SPI_SETTINGS|TIMOD01) ); /* Turn on the SPI */

	*pSPI_TDBR = SPI_RDSR;	/* send instruction to read status register */
	Wait_For_SPIF();		/*wait until the instruction has been sent*/
	dummyread = *pSPI_RDBR;	/*read dummy to empty the receive register*/
	*pSPI_TDBR = 0;			/*send dummy to receive the status register*/
	Wait_For_SPIF();		/*wait until the data has been sent*/
	status_register = *pSPI_RDBR;	/*read the status register*/
	
	SPI_OFF();		/* Turn off the SPI */


	for(n=0; n<1000; n++)
	{
		asm("nop;");
	}

	return status_register;
}

/*****************************************************************************
* ERROR_CODE ReadData()
*
* Read a value from flash for verify purpose
*
* Inputs:	unsigned long ulStart - holds the SPI start address
*			int pnData - pointer to store value read from flash
*			long lCount - number of elements to read
***************************************************************************** */
ERROR_CODE ReadData(  unsigned long ulStart, long lCount,char *pnData  )
{

	unsigned long ShiftValue;
	char *cnData,ReadValue;
	int i;

	cnData = (char *)pnData; /* Pointer cast to be able to increment byte wise */

	for (i=0; i<lCount; i++)
	{
		// Start SPI interface	
		SetupSPI( (COMMON_SPI_SETTINGS|TIMOD01) );

		*pSPI_TDBR = SPI_READ;			/* Send the read command to SPI device */
		Wait_For_SPIF();				/* Wait until the instruction has been sent */
		ShiftValue = ((ulStart+i) >> 8);	/* Send the highest byte of the 16 bit address at first	*/
		*pSPI_TDBR = ShiftValue;		/* Send the byte to the SPI device */
		Wait_For_SPIF();				/* Wait until the instruction has been sent */
		*pSPI_TDBR = (ulStart+i);			/* Send the lowest byte of the 16 bit address finally */
		Wait_For_SPIF();				/* Wait until the instruction has been sent */

		*pSPI_TDBR = 0;			/*send dummy*/
		Wait_For_SPIF();		/*wait until the data has been sent */
		ReadValue  = *pSPI_RDBR;	/*read */
		
		*cnData = ReadValue;	/* Store the value received in the buffer. */

		cnData++;				/* Increment the pointer of the buffer to store the next value */
		SPI_OFF();					/* Turn off the SPI */
	}

	for(i=0; i<1000; i++)
	{
		asm("nop;");
	}

	return NO_ERR;
}

ERROR_CODE WriteData ( unsigned long ulStartAddr, long lTransferCount, char *iDataSource )
{

	unsigned char ulWAddr;
	long lWTransferCount = 0;
	int i;
	char iData;
	ERROR_CODE ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash */

	
	/* Fourth, maximum number of 32 bytes will be taken from the Buffer
	   and sent to the SPI device. */
	for (i=0; i < lTransferCount; i++, lWTransferCount++)
	{
		/* First, a Write Enable Command must be sent to the SPI. */
		SendSingleCommand(SPI_WREN);
	
		/* Second, the SPI Status Register will be tested whether the 
		     Write Enable Bit has been set. */
		ErrorCode = Wait_For_WEL();

		if( POLL_TIMEOUT == ErrorCode )
			return ErrorCode;
	
		/* Third, the 16 bit address will be shifted out the SPI MOSI bytewise. */
		SetupSPI( (COMMON_SPI_SETTINGS|TIMOD01) ); /* Turns the SPI on */
		*pSPI_TDBR = SPI_PP;
		Wait_For_SPIF();		/* wait until the instruction has been sent */
		ulWAddr = (unsigned char)((ulStartAddr+i) >> 8);
		*pSPI_TDBR = ulWAddr;
		Wait_For_SPIF();		/* wait until the instruction has been sent */
		ulWAddr = (unsigned char)(ulStartAddr+i);
		*pSPI_TDBR = ulWAddr;
		Wait_For_SPIF();		/* wait until the instruction has been sent */
		iData = *iDataSource;
		*pSPI_TDBR = iData;
		Wait_For_SPIF();		/* wait until the instruction has been sent */

		((char *) iDataSource) ++;
		SPI_OFF(); /* Turns the SPI off */
		ErrorCode = Wait_For_Status(WIP);
	
		if( POLL_TIMEOUT == ErrorCode )
			return ErrorCode;

	}
	return ErrorCode;
}

#endif	/* CONFIG_SPI */
