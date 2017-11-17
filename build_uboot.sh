#!/bin/bash
export PATH=$TOPDIR/toolchains/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf:$PATH
cd u-boot-sunxi
./bpi-m2z.sh
./bpi-m2m.sh
./bpi-m2u.sh
./bpi-m2p.sh
./bpi-m3.sh
./bpi-m2.sh
./bpi-r1.sh
./bpi-m1p.sh
./bpi-m1.sh
