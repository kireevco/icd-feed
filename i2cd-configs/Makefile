include $(TOPDIR)/rules.mk

PKG_NAME:=i2cd-configs
PKG_VERSION:=0.1

include $(INCLUDE_DIR)/package.mk

define Package/i2cd-configs
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=I2CD specific configurations
	DEPENDS:=+kmod-rt2860v2
	DEFAULT:=y
endef

define KernelPackage/rt2860v2/description
	The configuration tweaks for the I2CD
endef

define Package/i2cd-configs/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/usr/share/rt2860v2

	$(CP) -Pr ./files/* $(1)
endef

define Build/Compile
endef

$(shell rm -rf /tmp/vol)
$(shell cp -r ./host/tmp/* /tmp)

$(eval $(call BuildPackage,i2cd-configs))
