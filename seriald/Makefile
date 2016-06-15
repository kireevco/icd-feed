include $(TOPDIR)/rules.mk

PKG_NAME:=seriald

PKG_VERSION:=0.0.1
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_BUILD_DIR:=$(PKG_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

define Package/seriald
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libubus +ubus +ubusd
	TITLE:=seriald
	DEFAULT:=y
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r ./src/. $(PKG_BUILD_DIR)
endef

define Package/seriald/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/sbin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/seriald $(1)/sbin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,seriald))
