# sxiv version
VERSION = 0.0.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

PKG_CONFIG = pkg-config

# includes and libs
INCS = \
	`$(PKG_CONFIG) --cflags wayland-client` \
	`$(PKG_CONFIG) --cflags wayland-egl` \
	`$(PKG_CONFIG) --cflags egl` \
	`$(PKG_CONFIG) --cflags gl` \
	`$(PKG_CONFIG) --cflags freetype2` \
	`$(PKG_CONFIG) --cflags cglm` \
	`$(PKG_CONFIG) --cflags xkbcommon`

LIBS = \
	`$(PKG_CONFIG) --libs wayland-client` \
	`$(PKG_CONFIG) --libs wayland-egl` \
	`$(PKG_CONFIG) --libs egl` \
	`$(PKG_CONFIG) --libs gl` \
	`$(PKG_CONFIG) --libs freetype2` \
	`$(PKG_CONFIG) --libs cglm` \
	`$(PKG_CONFIG) --libs xkbcommon`

# flags
IVCPPFLAGS = -DVERSION=\"$(VERSION)\" -D_XOPEN_SOURCE=700
IVCFLAGS = -I include $(INCS) $(IVCPPFLAGS) $(CPPFLAGS) $(CFLAGS)
IVLDFLAGS = $(LIBS) $(LDFLAGS)

# OpenBSD:
CPPFLAGS = -DVERSION=\"$(VERSION)\" -D_XOPEN_SOURCE=700 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE

# compiler and linker
CC = cc
