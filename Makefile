#
# Copyright(c) 2011-2025 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

CUR_MKFILE:= $(lastword $(MAKEFILE_LIST))

.PHONY: all clean rebuild QuoteGeneration QuoteVerification PCKCertSelection PCKRetrievalTool SGXPlatformRegistration PckClientTool WinPle WinPleIntel

all: QuoteGeneration QuoteVerification PCKCertSelection PCKRetrievalTool SGXPlatformRegistration PckClientTool WinPle WinPleIntel ThirdParty

ThirdParty:
	$(MAKE) -C external

QuoteGeneration: QuoteVerification
	$(MAKE) -C QuoteGeneration

QuoteVerification:
	$(MAKE) -C QuoteVerification

PCKCertSelection:
	$(MAKE) -C tools/PCKCertSelection

PCKRetrievalTool: QuoteGeneration
	$(MAKE) -C tools/PCKRetrievalTool

SGXPlatformRegistration: ThirdParty
	$(MAKE) -C tools/SGXPlatformRegistration

PckClientTool:
	$(MAKE) -C tools/PcsClientTool

WinPle:
	$(MAKE) -C driver/win/PLE

WinPleIntel:
	$(MAKE) -C driver/win/PLE INTEL_SIGNED=1

clean:
	$(MAKE) -C external clean
	$(MAKE) -C QuoteGeneration clean
	$(MAKE) -C QuoteVerification clean
	$(MAKE) -C tools/PCKCertSelection clean
	$(MAKE) -C tools/PCKRetrievalTool clean
	$(MAKE) -C tools/SGXPlatformRegistration clean
	$(MAKE) -C tools/PcsClientTool clean
	$(MAKE) -C driver/win/PLE clean
	$(MAKE) -C driver/win/PLE INTEL_SIGNED=1 clean

rebuild:
	$(MAKE) -f $(CUR_MKFILE) clean
	$(MAKE) -f $(CUR_MKFILE) all
