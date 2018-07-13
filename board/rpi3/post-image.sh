#!/bin/sh

ITS_FILE="${BR2_EXTERNAL_RPI3_OPTEE_PATH}/board/rpi3/rpi3_fit.its"
CONFIG_FILE="${BR2_EXTERNAL_RPI3_OPTEE_PATH}/board/rpi3/config.txt"

cd ${BINARIES_DIR}
echo $(pwd)

mkdir -p boot
cp image.fit boot/
cp u-boot-rpi.bin boot/
cp uboot-env.bin boot/
cp rpi-firmware/COPYING.linux boot/
cp rpi-firmware/LICENCE.broadcom boot/
cp rpi-firmware/bootcode.bin boot/
cp rpi-firmware/fixup.dat boot/
cp rpi-firmware/fixup_cd.dat boot/
cp rpi-firmware/fixup_db.dat boot/
cp rpi-firmware/fixup_x.dat boot/
cp rpi-firmware/start.elf boot/
cp rpi-firmware/start_cd.elf boot/
cp rpi-firmware/start_db.elf boot/
cp rpi-firmware/start_x.elf boot/
cp ${CONFIG_FILE} boot/

cd boot
tar -cvf ../bootfs.tar .