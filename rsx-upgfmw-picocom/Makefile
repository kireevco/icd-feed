#
# Copyright (C) 2016 Flo Technologies
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
# See LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rsx-upgfmw-picocom
PKG_VERSION:=1.4
PKG_RELEASE=$(PKG_SOURCE_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@github.com:FloTechnologies/icd-upgfmw-picocom.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=37e2567df5c222c402631c05e24238f9bdd453c9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/rsx-upgfmw-picocom
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Customized Picocom for rsx-upgfmw
	SUBMENU:=Terminal
endef

define Package/rsx-upgfmw-picocom/description
	minimal dumb-terminal emulation program
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" \
		picocom
endef

define Package/rsx-upgfmw-picocom/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/picocom $(1)/sbin/upgfmw-picocom
endef

$(eval $(call BuildPackage,rsx-upgfmw-picocom))
