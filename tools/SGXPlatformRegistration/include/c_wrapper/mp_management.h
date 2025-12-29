/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * File: mp_mangement.h 
 *   
 * Description: C definition of function wrappers for C++ methods in 
 * the MPManagement class implementation.
 */
#ifndef MP_MANAGEMENT_H_
#define MP_MANAGEMENT_H_

#include "MultiPackageDefs.h"

extern "C" 
{
	void     mp_management_init(const char* path);

	MpResult mp_management_get_package_info_key_blobs(uint8_t *buffer, uint16_t *size);
  MpResult mp_management_get_platform_manifest(uint8_t *buffer, uint32_t *size);
  MpResult mp_management_get_add_package(uint8_t *buffer, uint32_t *size);
  MpResult mp_management_get_registration_error_code(RegistrationErrorCode *error_code);
  MpResult mp_management_get_registration_status(MpTaskStatus *status);
  MpResult mp_management_get_registration_server_info(uint16_t *outFlags, char *outUrl, uint16_t *outUrlSize, uint8_t *outServerId, uint16_t *outServerIdSize);

  MpResult mp_management_get_sgx_status(MpSgxStatus *status);

  MpResult mp_management_set_registration_server_info(uint16_t flags, string url, const uint8_t *serverId, uint16_t serverIdSize);
  MpResult mp_management_set_membership_certificate(const uint8_t *buffer, uint16_t size);
    
	void mp_management_terminate();
};
#endif // #ifndef MP_MANAGEMENT_H_
