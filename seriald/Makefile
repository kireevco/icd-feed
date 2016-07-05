include $(TOPDIR)/rules.mk

PKG_NAME:=seriald
PKG_VERSION:=0.0.2
PKG_RELEASE=$(PKG_SOURCE_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@github.com:FloTechnologies/icd-seriald.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=16ae97af0dc93fd6a6df07569c3d66c0a681514f
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_LICENSE:=GPL-3.0

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/seriald
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libubox +libubus +ubus +ubusd
	TITLE:=seriald
	DEFAULT:=y
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/seriald/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/sbin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/seriald $(1)/sbin/
	$(CP) -Pr ./files/* $(1)
endef

$(eval $(call BuildPackage,seriald))
