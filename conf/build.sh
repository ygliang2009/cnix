#!/bin/bash
cd ..
PWD=`pwd`
PWD_OLD=$PWD
BASENAME=`basename $PWD_OLD`
NOW=`date +%Y%m%d%H%M%S`
TARNAME=OS_"$NOW".tar.gz
cd ..
rm -rf *.tar.gz
tar -zcvf $TARNAME $BASENAME
sz $TARNAME
echo "build finish"
