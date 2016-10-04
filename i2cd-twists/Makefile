include $(TOPDIR)/rules.mk

PKG_NAME:=i2cd-twists
PKG_VERSION:=0.1

include $(INCLUDE_DIR)/package.mk

define Package/i2cd-twists
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=I2CD specific configurations
	DEPENDS:=@TARGET_ramips +kmod-rt2800-soc +netifd +openssl-util +rsx-upgfmw +seriald
	DEFAULT:=y
endef

define Package/i2cd-twists/description
	The configurations for the I2CD
endef

define Package/i2cd-twists/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/lib/functions
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/usr/bin

	$(CP) -a ./files/* $(1)
endef

define Build/Compile
endef

$(shell rm -rf /tmp/vol)
$(shell cp -a ./host/tmp/* /tmp)

$(eval $(call BuildPackage,i2cd-twists))
