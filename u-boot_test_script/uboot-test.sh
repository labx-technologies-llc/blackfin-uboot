#!/bin/sh
USER=/home/test
CHECKOUT=$USER/checkouts
UBOOT_SCRIPTS=$USER/test_scripts/uboot
NOTIFY=grace.pan@analog.com
CVS_SERVER=10.99.22.20
CVS_ROOT=/cvsroot
BASELINE=standard
BOARD_TYPE=$1
VCO_MULT=$2
CCLK_DIV=$3
SCLK_DIV=$4

QUIT=2

## check out uboot
if [ ! -d $CHECKOUT ] ; then
  mkdir -p $CHECKOUT
fi
if [ ! -d $CHECKOUT/uboot ] ; then
  mkdir -p $CHECKOUT/uboot
fi

  $USER/test_scripts/uboot/change_clk.exp $BOARD_TYPE  $VCO_MULT  $CCLK_DIV  $SCLK_DIV
 
  export PATH=/opt/uClinux/bfin-uclinux/bin:/opt/uClinux/bfin-elf/bin:$PATH
  
if [ $BOARD_TYPE = BF533-STAMP ] ; then
 config_param=stamp_config 
elif [ $BOARD_TYPE = BF537-STAMP ] ; then
 config_param=bf537_config 
elif [ $BOARD_TYPE = BF561-EZKIT ] ; then
 config_param=ezkit561_config
elif [ $BOARD_TYPE = BF533-EZKIT ] ; then
 config_param=ezkit533_config
elif [ $BOARD_TYPE = BF537-EZKIT ] ; then
 config_param=stamp_config 
else 
echo "Unknown target board type $BOARD_TYPE."
exit
fi 

  echo "Make uBoot now."
  cd $CHECKOUT/uboot/u-boot_1.1.3
  make clean  1>/dev/null 2>/dev/null
  make mrproper  1>/dev/null 2>/dev/null
  make $config_param  1>/dev/null 2>/dev/null
  make  1>$UBOOT_SCRIPTS/uboot_make_log 2>$UBOOT_SCRIPTS/uboot_make_log
  cp u-boot.bin /tftpboot 

  cd $UBOOT_SCRIPTS   
  $UBOOT_SCRIPTS/network_restart.exp
  sleep 10
  $UBOOT_SCRIPTS/uboot_erase.exp

  sleep 10
  if [ -f $UBOOT_SCRIPTS/logs/summary ] ; then
       rm $UBOOT_SCRIPTS/logs/summary
  fi

  if [ -e $USER/test_scripts/test_result ] ; then
       rm $USER/test_scripts/test_result
  fi

  sleep 5
 
  cd $UBOOT_SCRIPTS
  $UBOOT_SCRIPTS/test-uboot.exp /dev/ttyS0 $BOARD_TYPE on on  1>/dev/null 2>/dev/null 
  sleep 500
  $UBOOT_SCRIPTS/test-uboot.exp /dev/ttyS0 $BOARD_TYPE on off  1>/dev/null 2>/dev/null
  sleep 500
  $UBOOT_SCRIPTS/test-uboot.exp /dev/ttyS0 $BOARD_TYPE off on  1>/dev/null 2>/dev/null
  sleep 500
  $UBOOT_SCRIPTS/test-uboot.exp /dev/ttyS0 $BOARD_TYPE off off  1>/dev/null 2>/dev/null

  cat $UBOOT_SCRIPTS/logs/summary

  cd $UBOOT_SCRIPTS
  if $UBOOT_SCRIPTS/decide_uboot_standard ; then
  echo  "success" > $USER/test_scripts/test_result
  else
  echo  "test_failed" > $USER/test_scripts/test_result
  fi

  echo
  echo  "The test_result of this testing:     "
  echo
  cat $USER/test_scripts/test_result

 
  sleep 1200
 
  
