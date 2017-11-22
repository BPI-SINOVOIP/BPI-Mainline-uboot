#!/bin/bash
TOPDIR=`pwd`
TOOLCHAIN=gcc-linaro-7.1.1-2017.08-x86_64_aarch64-linux-gnu
export PATH=$TOPDIR/toolchains/$TOOLCHAIN/bin:$PATH
cd u-boot-sunxi
./bpi-m64.sh
