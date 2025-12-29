/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/** File: utility.h
 *
 * Description: Definitions of some utility functions
 *
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#define ENCRYPTED_PPID_LENGTH             384
#define CPU_SVN_LENGTH                    16
#define ISV_SVN_LENGTH                    2
#define PCE_ID_LENGTH                     2
#define DEFAULT_PLATFORM_ID_LENGTH        16
#define PLATFORM_MANIFEST_LENGTH          1024 * 256 // 256K

/* PCE ID for the PCE in this library */
#define PCE_ID 0

/* Crypto_suite */
#define PCE_ALG_RSA_OAEP_3072 1

/* Signature_scheme */
#define PCE_NIST_P256_ECDSA_SHA256 0

#define PPID_RSA3072_ENCRYPTED  3

#define REF_RSA_OAEP_3072_MOD_SIZE   384 //hardcode n size to be 384
#define REF_RSA_OAEP_3072_EXP_SIZE     4 //hardcode e size to be 4

#include <stdint.h>
#include <string>
#include "network_wrapper.h"

typedef enum {
    UEFI_OPERATION_SUCCESS = 0,
    UEFI_OPERATION_UNEXPECTED_ERROR,
    UEFI_OPERATION_VARIABLE_NOT_AVAILABLE,
    UEFI_OPERATION_LIB_NOT_AVAILABLE,
    UEFI_OPERATION_FAIL
} uefi_status_t;

// for multi-package platform, get the platform manifet
// return value:
//  UEFI_OPERATION_SUCCESS: successfully get the platform manifest.
//  UEFI_OPERATION_VARIABLE_NOT_AVAILABLE: it means platform manifest is not avaible: it is not multi-package platform or platform manifest has been consumed.
//  UEFI_OPERATION_LIB_NOT_AVAILABLE: it means that the uefi shared library doesn't exist
//  UEFI_OPERATION_FAIL:  it is one add package request, now we don't support it. 
//  UEFI_OPERATION_UNEXPECTED_ERROR: error happens.
uefi_status_t get_platform_manifest(uint8_t ** buffer, uint32_t& out_buffer_size);

// for multi-package platform, set registration status 
// return value:
//  UEFI_OPERATION_SUCCESS: successfully set the platform's registration status.
//  UEFI_OPERATION_LIB_NOT_AVAILABLE: it means that the uefi shared library doesn't exist, maybe the registration agent package is not installed
//  UEFI_OPERATION_UNEXPECTED_ERROR: error happens.
uefi_status_t set_registration_status();

// return value:
//  0: successfully collect data
// -1: error happens.
int collect_data(uint8_t **p_data_buffer);

bool is_valid_proxy_type(std::string& proxy_type);

bool is_valid_use_secure_cert(std::string& use_secure_cert);

bool is_valid_tcb_update_type(std::string& tcb_update_type);


network_post_error_t generate_json_message_body(const uint8_t *raw_data, 
                                                const uint64_t raw_data_size,
                                                const uint16_t platform_id_length,
                                                const bool non_enclave_mode, 
                                                std::string &jsonString);

#ifdef _MSC_VER
bool get_program_path(TCHAR *p_file_path, size_t buf_size);
#else                                       
bool get_program_path(char *p_file_path, size_t buf_size);
#endif

#endif //UTILITY_H_
