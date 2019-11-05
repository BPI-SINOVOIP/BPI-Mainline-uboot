#!/bin/bash
TOPDIR=`pwd`
TOOLCHAIN=gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu
export PATH=$TOPDIR/toolchains/$TOOLCHAIN/bin:$PATH
#cd u-boot-sunxi
cd u-boot-2019.07
./bpi-m2p-h5.sh
./bpi-m64.sh
