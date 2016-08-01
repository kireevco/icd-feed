include $(TOPDIR)/rules.mk

PKG_NAME:=rsx-upgfmw
PKG_VERSION:=0.0.1
PKG_RELEASE=$(PKG_SOURCE_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@github.com:FloTechnologies/icd-upgfmw.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=345678f2a38b865162c7b59cb3eed814b526c8c9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_LICENSE:=GPL-3.0

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/rsx-upgfmw
	SECTION:=utils
	CATEGORY:=Utilities
	DEPENDS:=+picocom
	TITLE:=Rosonix ZigBee Module RX-357x Firmware Upgrader
	DEFAULT:=y
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/rsx-upgfmw/install
	$(INSTALL_DIR) $(1)/sbin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/upgfmw $(1)/sbin/
endef

$(eval $(call BuildPackage,rsx-upgfmw))
