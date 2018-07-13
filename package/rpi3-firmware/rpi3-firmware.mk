RPI3_FIRMWARE_VERSION = 1.20170215
RPI3_FIRMWARE_SITE = $(call github,raspberrypi,firmware,$(RPI3_FIRMWARE_VERSION))
RPI3_FIRMWARE_INSTALL_IMAGES = YES

define RPI3_FIRMWARE_INSTALL_IMAGES_CMDS
	cp -r $(@D)/boot $(BINARIES_DIR)/rpi-firmware
endef

$(eval $(generic-package))
