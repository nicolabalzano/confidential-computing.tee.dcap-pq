#
# Copyright(c) 2011-2025 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

# -----------------------------------------------------------------------------
# Function : root-dir
# Arguments: 1: path
# Returns  : Parent dir or path of $1, with final separator removed.
# -----------------------------------------------------------------------------
root-dir = $(patsubst %/,%,$(dir $(1:%/=%)))

# -----------------------------------------------------------------------------
# Macro    : cur-dir
# Returns  : the directory of the current Makefile
# Usage    : $(cur-dir)
# -----------------------------------------------------------------------------
cur-dir = $(call root-dir,$(lastword $(MAKEFILE_LIST)))


CUR_DIR              := $(call cur-dir)


include $(CUR_DIR)/../QuoteGeneration/buildenv.mk

MODE					?= HW
DEBUG					?= 0
DCAP_QG_DIR				:= $(ROOT_DIR)
PREBUILD_PATH	        := $(DCAP_QG_DIR)/../prebuilt
DCAP_QV_DIR				:= $(DCAP_QG_DIR)/../QuoteVerification
QVL_SRC_PATH 			?= $(DCAP_QV_DIR)/QVL/Src
SGXSSL_PACKAGE_PATH 	?= $(DCAP_QV_DIR)/sgxssl/Linux/package
PREBUILD_OPENSSL_PATH	?= $(PREBUILD_PATH)/openssl

SGX_COMMON_CFLAGS := $(COMMON_FLAGS) -m64 -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants
SGX_COMMON_CXXFLAGS := $(COMMON_FLAGS) -m64 -Wnon-virtual-dtor -std=c++17

DCAP_EXTERNAL_DIR       := $(DCAP_QG_DIR)/../external

QVL_LIB_PATH := $(QVL_SRC_PATH)/AttestationLibrary
QVL_PARSER_PATH := $(QVL_SRC_PATH)/AttestationParsers
QVL_COMMON_PATH := $(QVL_SRC_PATH)/AttestationCommons

ifdef SERVTD_ATTEST
COMMON_INCLUDE := -I$(ROOT_DIR)/../../../common/inc/ -I$(ROOT_DIR)/../../../common/inc/tlibc -I$(ROOT_DIR)/../../../sdk/tlibcxx/include -isystem$(SGXSSL_PACKAGE_PATH)/include
else
COMMON_INCLUDE := -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -isystem$(SGXSSL_PACKAGE_PATH)/include
endif

QVL_LIB_INC := -I$(QVL_COMMON_PATH)/include -I$(QVL_COMMON_PATH)/include/Utils -I$(QVL_LIB_PATH)/include -I$(QVL_LIB_PATH)/src -I$(QVL_PARSER_PATH)/include -I$(QVL_SRC_PATH)/ThirdParty/rapidjson/include  -isystem$(DCAP_EXTERNAL_DIR)/jwt-cpp/include

QVL_PARSER_INC := -I$(QVL_COMMON_PATH)/include -I$(QVL_COMMON_PATH)/include/Utils -I$(QVL_SRC_PATH) -I$(QVL_PARSER_PATH)/include -I$(QVL_PARSER_PATH)/src -I$(QVL_LIB_PATH)/include -isystem$(QVL_SRC_PATH)/ThirdParty/rapidjson/include

QVL_LIB_FILES := $(sort $(wildcard $(QVL_LIB_PATH)/src/*.cpp) $(wildcard $(QVL_LIB_PATH)/src/*/*.cpp) $(wildcard $(QVL_LIB_PATH)/src/*/*/*.cpp) $(wildcard $(QVL_COMMON_PATH)/src/Utils/*.cpp))
QVL_PARSER_FILES := $(sort $(wildcard $(QVL_PARSER_PATH)/src/*.cpp) $(wildcard $(QVL_PARSER_PATH)/src/*/*.cpp))
