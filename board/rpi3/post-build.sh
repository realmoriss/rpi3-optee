#!/bin/sh

KEY_PATH="${BR2_EXTERNAL_RPI3_OPTEE_PATH}/board/rpi3/rootfs-overlay/etc/ssh"
TARGET_KEY_PATH="${TARGET_DIR}/etc/ssh/"

if [ ! -f "${KEY_PATH}/ssh_host_rsa_key" ]; then
	ssh-keygen -t rsa -b 2048 -f "${KEY_PATH}/ssh_host_rsa_key" -N ""
	cp ${KEY_PATH}/ssh_host_rsa_key ${TARGET_KEY_PATH}
fi

if [ ! -f "${KEY_PATH}/ssh_host_dsa_key" ]; then
	ssh-keygen -t dsa -b 1024 -f "${KEY_PATH}/ssh_host_dsa_key" -N ""
	cp ${KEY_PATH}/ssh_host_dsa_key ${TARGET_KEY_PATH}
fi

if [ ! -f "${KEY_PATH}/ssh_host_ecdsa_key" ]; then
	ssh-keygen -t ecdsa -b 256 -f "${KEY_PATH}/ssh_host_ecdsa_key" -N ""
	cp ${KEY_PATH}/ssh_host_ecdsa_key ${TARGET_KEY_PATH}
fi

if [ ! -f "${KEY_PATH}/ssh_host_ed25519_key" ]; then
	ssh-keygen -t ed25519 -b 2048 -f "${KEY_PATH}/ssh_host_ed25519_key" -N ""
	cp ${KEY_PATH}/ssh_host_ed25519_key ${TARGET_KEY_PATH}
fi

GETTY_LINE="tty1::respawn:/sbin/getty -L tty1 0 xterm-256color"
INITTAB_PATH="${TARGET_DIR}/etc/inittab"

if ! grep -q ${GETTY_LINE} ${INITTAB_PATH}; then
	echo ${GETTY_LINE} >> ${INITTAB_PATH}
fi
