/***********************************************************************************************************
#
#                        PROJECT                 :       BFIN
#                        VERISON                 :       2.0
#                        FILE                    :       u-boot.lds
#                        MODIFIED DATE           :       29 jun 2004
#                        AUTHOR                  :       BFin Project-ADI
#                        LOCATION                :       LG Soft India,Bangalore
#
***********************************************************************************************************/

OUTPUT_FORMAT("elf32-little-nisa")
OUTPUT_ARCH(nisa)
ENTRY(start)
MEMORY 
	{
	ram		: ORIGIN = 0x1800000,  	LENGTH = 0x1000000
	flash		: ORIGIN = 0x20000000, 	LENGTH = 0x80000
	}

SECTIONS
{
   . = 0x20000000;  
   .text : {
     cpu/bf533/start.o (.text)
   } > flash

  . = ALIGN(4);  
  . = 0x1800000;
  .text1 : {
      cpu/bf533/start1.o
    *(.text)	
    *(.fixup)
    *(.got1)
    . = ALIGN(4);
    *(.rodata)
    *(.rodata1)
    *(.rodata.str1.4)
	} > ram 

  . = ALIGN(4);  
  .data :
	{
	_sdata = . ;
	__data_start = . ;
     	*(.rodata)
	*(.data)
      	*(.data.*)	
      	. = ALIGN(16) ;
     	ramdisk_begin = .;
      	*(.ramdisk)
      	ramdisk_end = .;

      	*(.setup.init)		
      	*(.exitcall.exit)		
	
	__u_boot_cmd_start = .;
	  *(.u_boot_cmd)
	__u_boot_cmd_end = .;

	_edata = . ;
	edata = . ;
	 } > ram 
	
	. = 0x1800000;	/* Ram data segment to use */
	_bf533_data_dest = ABSOLUTE(.);
	_bf533_data_start = LOADADDR(.data);
	
	. = ALIGN(4);
	.got : AT ( LOADADDR(.data) + SIZEOF(.data) ) { *(.got) }
	_bf533_data_size = SIZEOF(.text1) + SIZEOF(.data) + SIZEOF(.got);

  .bss :
    {
	  . = ALIGN(16) ;
	  _sbss = . ;
	  __bss_start = . ;
	  __data_end = . ;
	  *(.bss)
	  *(COMMON)
	  . = ALIGN(16) ;
	  end = . ;
	  _ebss = . ;	  
	  _end = . ;
    } > ram 

  
  . = 0x2000000;
  .eram :
    {
	__ramend = . ;
    } > ram
}
