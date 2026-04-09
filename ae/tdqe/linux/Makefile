#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

TOP_DIR := ../../../QuoteGeneration
include $(TOP_DIR)/../ae/dep/buildenv.mk

AENAME := tdqe
SONAME := $(AENAME).so
SIGNED_TDQE_NAME := libsgx_$(AENAME)$(if $(FIPS),-fips).signed.so
TDQE_CONFIG_FILE := config$(if $(FIPS),.fips).xml
TDQE_KEY_FILE := ../../dep/dcap_ae_test_key.pem

TCRYPTO_LIB_NAME := sgx_tcrypto

INCLUDE := -I$(SGX_SDK)/include							\
           -I$(SGX_SDK)/include/tlibc					\
           -I$(SGX_SDK)/include/libcxx					\
           -I$(COMMON_DIR)/inc/internal					\
           -I$(COMMON_DIR)/inc/internal/linux			\
		   -I$(TOP_DIR)/../ae/dep/inc/internal			\
           -I$(TOP_DIR)/quote_wrapper/common/inc		\
           -I$(TOP_DIR)/pce_wrapper/inc					\
           -I$(TOP_DIR)/../ae/pq/mldsa-native/mldsa		\
           -I$(TOP_DIR)/../ae/pq/mldsa-native/mldsa/src	\
           -I..											\
           -I.

EXTERNAL_LIB = $(EXTERNAL_LIB_NO_CRYPTO) -lsgx_tcxx -l$(TCRYPTO_LIB_NAME)

vpath %.cpp .. .
CPP_SRC := quoting_enclave_tdqe.cpp
C_SRC := tdqe_mldsa_adapter.c mldsa_native.c

OBJ := $(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)
OBJS := $(sort $(OBJ) version.o)

.PHONY: all
all: $(SONAME) $(SIGNED_TDQE_NAME)

$(SONAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -nostdlib -nodefaultlibs -nostartfiles -Wl,-soname=libsgx_${AENAME}.signed.so.$(call get_major_version,TDQE_VERSION) $(LDTFLAGS) -fno-exceptions -fno-rtti
	$(STRIP) --strip-unneeded --remove-section=.comment --remove-section=.note $@

$(SIGNED_TDQE_NAME): $(SONAME) $(TDQE_CONFIG_FILE) $(TDQE_KEY_FILE)
	@$(SGX_ENCLAVE_SIGNER) sign -key $(TDQE_KEY_FILE) -enclave $< -out $@ -config $(TDQE_CONFIG_FILE)
	@echo "SIGN =>  $@"

quoting_enclave_tdqe.o: $(AENAME)_t.c

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(DEFINES) -fno-exceptions -c $< -o $@

tdqe_mldsa_adapter.o: ../tdqe_mldsa_adapter.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINES) -c $< -o $@

mldsa_native.o: $(TOP_DIR)/../ae/pq/mldsa-native/mldsa/mldsa_native.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINES) -U__CET__ -c $< -o $@

$(AENAME)_t.c: ../$(AENAME).edl
	@$(SGX_EDGER8R) --trusted $<

.PHONY: clean
clean:
	@$(RM) *.o
	@$(RM) *.so
	@$(RM) *.map
	@$(RM) *_t.*
	@$(RM) *.signed.so

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all
