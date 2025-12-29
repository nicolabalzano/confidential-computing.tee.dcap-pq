/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * File: MPManagement.h 
 *   
 * Description: Classe definition for the management tool functionality. 
 */
#ifndef MPMANAGEMENT_H_
#define MPMANAGEMENT_H_

#include <string>
#include "MultiPackageDefs.h"

using std::string;
class MPUefi;

/**
 * This is the main entry point for the Multi-Package Management interface library.
 * Used to manage the registration SGX feature and registration flow.
 */

class MPManagement {
    public:
		MPManagement(string uefi_path);

        // Retrieves KeyBlobs.
        // if KeyBlobs UEFI are ready for reading, copies the keyblobs to input buffer and sets the package info status to completed.
        // if not, returns an appropriate error (MP_NO_PENDING_DATA). populates buffer_size with the required size in case of insufficient size.
        virtual MpResult getPackageInfoKeyBlobs(uint8_t *buffer, uint16_t &buffer_size);

        // Retrieves PlatformManifest.
        // if PlatformManifest UEFI are ready for reading, copies the PlatformManifest to input buffer and sets the registration status to completed.
        // if not, returns an appropriate error (MP_NO_PENDING_DATA). populates buffer_size with the required size in case of insufficient size.
        virtual MpResult getPlatformManifest(uint8_t *buffer, uint32_t &buffer_size);

        // Retrieves Add Package request.
        // If SgxRegistrationServerRequest UEFI variable is available and contains a pending Add Package request, copies the ADD_REQUEST to output buffer.
        // If not, returns an appropriate error (MP_NO_PENDING_DATA). Populates buffer_size with the required size in case of insufficient size.
        virtual MpResult getAddPackageRequest(uint8_t *buffer, uint32_t &buffer_size);

        // Sets the SGX Registration Server response to BIOS.
        // The input buffer should contain the platform membership certificates as-received from the registration server
        // after a successful "Add Package".
        virtual MpResult setMembershipCertificates(const uint8_t *membershipCertificates, uint16_t membershipCertificatesSize);

        // Retrieves SGX registration information as configured in the the BIOS.
        // Pass-through for the UEFI library function getRegistrationServerInfo
        virtual MpResult getRegistrationServerInfo(uint16_t &flags, string &outUrl, uint8_t *serverId, uint16_t &serverIdSize);
        
        // Retrieves registration error code.
        // If registration is completed successfully, error_code will be set to 0.
        // If registration process failed, error_code will be set to the relevant last reported error code.
        virtual MpResult getRegistrationErrorCode(RegistrationErrorCode &error_code);
        
        // Retrieves registration error code.
        // If registration is completed successfully, error_code will be set to 0.
        // If registration process failed, error_code will be set to the relevant last reported error code.
        virtual MpResult getRegistrationStatus(MpTaskStatus &status);
        
        // Retrieves SGX status.
        // If the platform supports SGX, returns SGX status as configured by BIOS.
        // If failed returns an appropriate error.
        virtual MpResult getSgxStatus(MpSgxStatus &status);
        

        // Sets registration server info.
        // If registration is completed successfully, error_code will be set to 0.
        // If registration failed, error_code will be set to the relevant error.
        virtual MpResult setRegistrationServerInfo(const uint16_t &flags, const string &url, const uint8_t *serverId, const uint16_t &serverIdSize);
        virtual ~MPManagement();
    private:
        MPUefi *m_mpuefi{nullptr};

        MPManagement& operator=(const MPManagement&) {return *this;}
        MPManagement(const MPManagement& src) {(void) src; }
        MpResult getRequestData(uint8_t *buffer, uint32_t &buffer_size, MpRequestType requestType);
};

#endif  // #ifndef MPMANAGEMENT_H_
