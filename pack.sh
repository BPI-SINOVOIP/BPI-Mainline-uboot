#!/bin/bash
# (c) 2015, Leo Xu <otakunekop@banana-pi.org.cn>
# Build script for BPI-M2P-BSP 2016.03.02

TOPDIR=`pwd`
T="$TOPDIR"
BOARD="bpi-all"
ARCH=arm
KERNEL=zImage
KVER=5.1.1
K="$T/linux-${KVER}"
kernel="${KVER}-BPI-Kernel"
uboot="u-boot-2019.07"
EXTLINUX=bananapi/${BOARD}/linux4/

echo "top dir $T"

cp_download_files()
{
SD="$T/SD_${KVER}/${BOARD}"
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
	cp -a $T/${uboot}/out/*.img.gz $U
	#
	## copy files to BPI-BOOT
	#
	mkdir -p $B/$EXTLINUX/extlinux/dtb
	cp -a $T/extlinux/${BOARD}/* $B/$EXTLINUX/extlinux
	cp -a $K/output/${BOARD}/arch/${ARCH}/boot/${KERNEL} $B/$EXTLINUX/extlinux/${KERNEL}
	cp -a $K/output/${BOARD}/arch/${ARCH}/boot/dts/* $B/$EXTLINUX/extlinux/dtb
	rm -f $B/$EXTLINUX/extlinux/dtb/overlay/.sun*
	mkdir -p $B/efi
	cp -a $T/efi/* $B/efi

	#
	## copy files to BPI-ROOT
	#
	mkdir -p $R/usr/lib/u-boot/bananapi/${uboot}
	cp -a $U/*.gz $R/usr/lib/u-boot/bananapi/${uboot}
	#
	## copy files to BPI-ROOT/boot
	#
	rm -rf $R/boot
	mkdir -p $R/boot
	cp -a $K/output/${BOARD}/arch/${ARCH}/boot/${KERNEL} $R/boot/vmlinuz-5.1.1-bpi
	mkdir -p $R/efi
	cp -a $T/efi/* $R/efi
	#
	## copy files to BPI-ROOT/lib/modules
	#
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
	(cd $R ; tar czvf $SD/${kernel}.tgz efi boot lib/modules)
	(cd $R ; mv $R/net lib/modules/${kernel}/kernel/net)
	# BPI-ROOT: BOOTLOADER
	(cd $R ; tar czvf $SD/BOOTLOADER-${BOARD}-linux4.tgz usr/lib/u-boot/bananapi)


	return #SKIP
}

cp_download_files

echo -e "\033[31m PACK success!\033[0m"
echo
