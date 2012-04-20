#!/bin/bash

cd ../logs
 
if [ ! -f summary ] ; then
  touch summary
  chmod 777 summary
fi

part_summary_file=$1

cat $part_summary_file >> summary
