RPI3_HEAD_BIN_VERSION = 1.0
RPI3_HEAD_BIN_SITE_METHOD = local
RPI3_HEAD_BIN_SITE = $(RPI3_HEAD_BIN_PKGDIR)/sources
RPI3_HEAD_BIN_DEPENDENCIES = uboot
RPI3_HEAD_BIN_INSTALL_IMAGES = YES

define RPI3_HEAD_BIN_BUILD_CMDS
	$(TARGET_CROSS)as $(@D)/head.S -o $(@D)/head.o
	$(TARGET_CROSS)objcopy -O binary $(@D)/head.o $(@D)/head.bin
endef

define RPI3_HEAD_BIN_INSTALL_IMAGES_CMDS
	$(INSTALL) -m 755 -D $(@D)/head.bin $(BINARIES_DIR)/head.bin
	cat $(@D)/head.bin $(BINARIES_DIR)/u-boot.bin > $(@D)/u-boot-rpi.bin
	$(INSTALL) -m 755 -D $(@D)/u-boot-rpi.bin $(BINARIES_DIR)/u-boot-rpi.bin
endef

$(eval $(generic-package))
