#!/bin/bash
TOPDIR=`pwd`
TOOLCHAIN=gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
export PATH=$TOPDIR/toolchains/$TOOLCHAIN/bin:$PATH
cd u-boot-sunxi
./bpi-m2p-h5.sh
./bpi-m64.sh
