#!/bin/bash
TOPDIR=`pwd`
TOOLCHAIN=gcc-linaro-6.4.1-2017.11-x86_64_aarch64-linux-gnu
export PATH=$TOPDIR/$TOOLCHAIN/bin:$PATH
cd arm-trusted-firmware/sunxi
./bpi-sunxi64.sh
