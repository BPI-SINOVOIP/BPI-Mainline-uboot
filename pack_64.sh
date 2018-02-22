#!/bin/bash
# (c) 2015, Leo Xu <otakunekop@banana-pi.org.cn>
# Build script for BPI-M2P-BSP 2016.03.02

TOPDIR=`pwd`
T="$TOPDIR"
BOARD="bpi-64"
ARCH=arm64
KERNEL=Image
K="$T/../BPI-Mainline-kernel/linux-4.14"
kernel="4.14.20-BPI-64-Kernel"
uboot="u-boot-2018.01"
EXTLINUX=bananapi/${BOARD}/linux4/

echo "top dir $T"

cp_download_files()
{
SD="$T/SD/${BOARD}"
U="${SD}/100MB"
B="${SD}/BPI-BOOT"
R="${SD}/BPI-ROOT"
	#
	## clean SD dir.
	#
	rm -rf $SD
	#
	## create SD dirs (100MB, BPI-BOOT, BPI-ROOT) 
	#
	mkdir -p $SD
	mkdir -p $U
	mkdir -p $B
	mkdir -p $R
	#
	## copy files to 100MB
	#
	#cp -a $T/out/100MB/* $U
	cp -a $T/u-boot-sunxi/out/*.img.gz $U
	#
	## copy files to BPI-BOOT
	#
	mkdir -p $B/$EXTLINUX/extlinux/dtb
	cp -a $T/extlinux/${BOARD}/* $B/$EXTLINUX/extlinux
	cp -a $K/output/${BOARD}/arch/${ARCH}/boot/${KERNEL} $B/$EXTLINUX/extlinux/${KERNEL}
	cp -a $K/output/${BOARD}/arch/${ARCH}/boot/dts/allwinner $B/$EXTLINUX/extlinux/dtb/allwinner
	rm -f $B/$EXTLINUX/extlinux/dtb/allwinner/.sun*
	rm -f $B/$EXTLINUX/extlinux/dtb/allwinner/overlay/.sun*

	#
	## copy files to BPI-ROOT
	#
	mkdir -p $R/usr/lib/u-boot/bananapi/${uboot}
	cp -a $U/*.gz $R/usr/lib/u-boot/bananapi/${uboot}
	rm -rf $R/lib/modules
	mkdir -p $R/lib/modules
	cp -a $K/output/${BOARD}/out/lib/modules/${kernel} $R/lib/modules
	#
	## create files for bpi-tools & bpi-migrate
	#
	# BPI-BOOT
	(cd $B ; tar czvf $SD/BPI-BOOT-${BOARD}-linux4.tgz .)
	# BPI-ROOT: kernel modules
	#(cd $R ; tar czvf $SD/${kernel}.tgz lib/modules)
	(cd $R ; tar czvf $SD/${kernel}-net.tgz lib/modules/${kernel}/kernel/net)
	(cd $R ; mv lib/modules/${kernel}/kernel/net $R/net)
	(cd $R ; tar czvf $SD/${kernel}.tgz lib/modules)
	(cd $R ; mv $R/net lib/modules/${kernel}/kernel/net)
	# BPI-ROOT: BOOTLOADER
	(cd $R ; tar czvf $SD/BOOTLOADER-${BOARD}-linux4.tgz usr/lib/u-boot/bananapi)


	return #SKIP
}

cp_download_files

echo -e "\033[31m PACK success!\033[0m"
echo
