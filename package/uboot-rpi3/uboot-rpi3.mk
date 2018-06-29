UBOOT_RPI3_VERSION = bc6d93cf5eee9fc979a7c5141ff6312bed2835a2
UBOOT_RPI3_SITE = "git://github.com/linaro-swg/u-boot.git"
UBOOT_RPI3_INSTALL_TARGET = YES
UBOOT_RPI3_INSTALL_HOST = YES
UBOOT_RPI3_INSTALL_IMAGES = YES
UBOOT_RPI3_DEPENDENCIES = linux optee-os

UBOOT_EXT_DTB = $(BINARIES_DIR)/bcm2710-rpi-3-b.dtb

UBOOT_RPI3_BUILD_PARAMS = CROSS_COMPILE=$(TARGET_CROSS) ARCH=arm \
	EXT_DTB=$(UBOOT_EXT_DTB)

UBOOT_RPI3_SIGNED_BUILD_PARAMS = CROSS_COMPILE=$(TARGET_CROSS) ARCH=arm \
	EXT_DTB=$(@D)/signed.dtb

define UBOOT_RPI3_BUILD_CMDS
	cp $(UBOOT_RPI3_PKGDIR)/sources/head.S $(@D)/
	cp $(UBOOT_RPI3_PKGDIR)/sources/rpi3_fit.its $(@D)/
	$(TARGET_CROSS)as $(@D)/head.S -o $(@D)/head.o
	$(TARGET_CROSS)objcopy -O binary $(@D)/head.o $(@D)/head.bin
	$(MAKE) -C $(@D) $(UBOOT_RPI3_BUILD_PARAMS) rpi_3_defconfig
	$(MAKE) -C $(@D) $(UBOOT_RPI3_BUILD_PARAMS) tools
	$(@D)/tools/mkenvimage -s $(BR2_PACKAGE_UBOOT_RPI3_ENVIMAGE_SIZE) -o $(@D)/uboot-env.bin $(BR2_PACKAGE_UBOOT_RPI3_ENVIMAGE_SOURCE)
	mkdir -p $(@D)/keys
	openssl genrsa -F4 -out $(@D)/keys/dev.key 2048
	openssl req -batch -new -x509 -key $(@D)/keys/dev.key -out $(@D)/keys/dev.crt
	cp $(UBOOT_EXT_DTB) $(@D)/signed.dtb
	cp $(BINARIES_DIR)/optee.bin $(@D)/
	cp $(BINARIES_DIR)/Image $(@D)/
	$(@D)/tools/mkimage -f $(@D)/rpi3_fit.its -K $(@D)/signed.dtb -k $(@D)/keys $(@D)/image.fit
	$(@D)/tools/fit_check_sign -f $(@D)/image.fit -k $(@D)/signed.dtb
	$(MAKE) -C $(@D) $(UBOOT_RPI3_SIGNED_BUILD_PARAMS) all
	cat $(@D)/head.bin $(@D)/u-boot.bin > $(@D)/u-boot-rpi.bin
endef

define UBOOT_RPI3_INSTALL_IMAGES_CMDS
	$(INSTALL) -m 755 -D $(@D)/u-boot-rpi.bin $(BINARIES_DIR)/
	$(INSTALL) -m 755 -D $(@D)/uboot-env.bin $(BINARIES_DIR)/
	$(INSTALL) -m 755 -D $(@D)/image.fit $(BINARIES_DIR)/
endef

$(eval $(generic-package))
