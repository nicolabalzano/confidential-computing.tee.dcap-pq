#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

include installConfig

USR_LIB_PATH=$(shell readlink -m $(DESTDIR)/usr/$(notdir $(shell gcc -print-multi-os-directory))/$(shell dpkg-architecture -qDEB_HOST_MULTIARCH 2> /dev/null))

USR_LIB_VER=1.0.0
SPLIT_VERSION=$(word $2,$(subst ., ,$1))

default:

install:
	install -d $(USR_LIB_PATH)
	install ${LIB_DIR}/* $(USR_LIB_PATH)
	cd $(USR_LIB_PATH) && \
	mv libsgx_qae.signed.so libsgx_qae.signed.so.$(USR_LIB_VER) && \
	ln -fs libsgx_qae.signed.so.$(USR_LIB_VER) libsgx_qae.signed.so.$(call SPLIT_VERSION,$(USR_LIB_VER),1) && \
	ln -fs libsgx_qae.signed.so.$(call SPLIT_VERSION,$(USR_LIB_VER),1) libsgx_qae.signed.so
