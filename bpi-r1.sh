export ARCH=arm
KBUILD_OUTPUT=out/bpi-r1
export KBUILD_OUTPUT
mkdir -p $KBUILD_OUTPUT
export CROSS_COMPILE=arm-linux-gnueabihf-
make Bananapi_R1_defconfig
make -j8
