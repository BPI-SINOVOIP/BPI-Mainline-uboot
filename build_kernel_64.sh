#!/bin/bash
TOPDIR=`pwd`
export PATH=$TOPDIR/toolchains/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin:$PATH
#cd linux-4.14
#cd linux-4.17.6
#cd linux-4.18-rc4
#cd linux-4.18
#cd linux-4.19-rc1
#cd linux-4.19
K=linux-5.1.1
cd $K
date >> $K.log
echo build $K >>$K.log
./build_64.sh
date >> $K.log
