export ARCH=arm
KBUILD_OUTPUT=out/bpi-m2
export KBUILD_OUTPUT
mkdir -p $KBUILD_OUTPUT
export CROSS_COMPILE=arm-linux-gnueabihf-
make Bananapi_M2_defconfig
make -j8
