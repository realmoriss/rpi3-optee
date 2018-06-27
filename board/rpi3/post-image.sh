#!/bin/sh

ITS_FILE="${BR2_EXTERNAL_RPI3_OPTEE_PATH}/board/rpi3/rpi3_fit.its"
CONFIG_FILE="${BR2_EXTERNAL_RPI3_OPTEE_PATH}/board/rpi3/config.txt"
MKIMAGE="${HOST_DIR}/bin/mkimage"

cd ${BINARIES_DIR}
echo $(pwd)
mkdir -p "keys"

openssl genrsa -F4 -out "keys/dev.key" 2048
openssl req -batch -new -x509 -key "keys/dev.key" -out "keys/dev.crt"

cp ${ITS_FILE} "rpi3_fit.its"
${MKIMAGE} -f "rpi3_fit.its" -k keys -K "bcm2710-rpi-3-b.dtb" -r "image.fit"

mkdir -p boot
cp image.fit boot/
cp u-boot-rpi.bin boot/
cp uboot-env.bin boot/
cp rpi-firmware/* boot/
cat ${CONFIG_FILE} > boot/config.txt