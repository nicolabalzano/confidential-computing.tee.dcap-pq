/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * File: MPNetwork.h 
 *   
 * Description: Classe definition for the UEFE accessor functionality for 
 * communicatinge to the BIOS. 
 */
#ifndef MP_UEFI_H__
#define MP_UEFI_H__

#include <memory>
#include <string>
#include "IUefi.h"
#include "MultiPackageDefs.h"

#ifdef _WIN32
#define MPUefiDllExport  __declspec(dllexport)
#else
#define MPUefiDllExport
#endif

/**
 * This is the main entry point for the SGX Multi-Package UEFI CPP interface.
 * Used to get and set various UEFI variables for the Multi-Package registration flows.
 */

MPUefiDllExport std::unique_ptr<IUefi> initUefi(std::string path, LogLevel logLevel = MP_REG_LOG_LEVEL_ERROR);

class  MPUefi final {
    public:
        /**
         * MPUefi class constructor
         *
         * @param path      - input parameter, Linux absolute path to the UEFI variables directory.
         *                    For Windows, this parameter should be empty.
         * @param logLevel  - input parameter, desired logging level.
         */
        MPUefiDllExport MPUefi(std::unique_ptr<IUefi> uefi);

        /**
         * Retrieves the pending request type.
         * The BIOS generates a request when there is a pending request to be sent to the SGX Registration Server.
         *
         * @param type - output parameter, holds the pending request type or MP_REQ_NONE.       
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult getRequestType(MpRequestType &type);

        /**
         * Retrieves the content of a pending request.
         * The request should be sent to the SGX Registration Server.
         * The BIOS generates PlatformManifest request for first "Platform Binding" and "TCB Recovery".
         * The BIOS generates AddPackage request to register a new "Added Package".
         *
         * @param request       - output parameter, holds the request buffer to be populated.
         * @param requestSize   - input parameter, size of request buffer in bytes.
         *                      - output paramerter, holds the actual size written to request buffer.
         *                        if response equals MP_USER_INSUFFICIENT_MEM or if request buffer is NULL, holds the pending request size.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_NO_PENDING_DATA
         *      - MP_USER_INSUFFICIENT_MEM
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult getRequest(uint8_t *request, uint32_t &requestSize);

        /**
         * Sets the content of a received response.
         * The response should be received from the SGX Registration Server.
         * Platform membership certificates response returned from the registration server after a successful "Add Package".
         *
         * @param response      - input parameter, holds the response buffer to be set.
         * @param responseSize  - input parameter, size of response buffer in bytes.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult setServerResponse(const uint8_t *response, const uint16_t &responseSize);

        /**
         * Retrieves the encrypted key blobs.
         * BIOS generates this data when a new or modified KEY_BLOBs are available.
         *
         * @param blobs         - output parameter, holds the blobs buffer to be populated.
         * @param blobsSize     - input parameter, size of blobs buffer in bytes.
         *                      - output paramerter, holds the actual size written to blobs buffer.
         *                        if response equals MP_USER_INSUFFICIENT_MEM or if blobs buffer is NULL, holds the pending blobs size.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_NO_PENDING_DATA
         *      - MP_USER_INSUFFICIENT_MEM
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult getKeyBlobs(uint8_t *blobs, uint16_t &blobsSize);
        
        /**
         * Retrieves the current registration status.
         *
         * @param status    - output parameter, holds the current registration status.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult getRegistrationStatus(MpRegistrationStatus &status);

        /**
         * Sets the registration status.
         *
         * @param status    - input parameter, holds the desired registration status.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult setRegistrationStatus(const MpRegistrationStatus &status);

        /**
         * Retrieves the registration server information.
         *
         * @param flags         - output parameter, holds the retrieved registration flags.
         *                        If BIT[0] is set, the registration agent will not send the registration request to the registration server.
         *                        The user will have to read the request and send it to the registration server.
         * @param serverAddress - output parameter, holds the registration server address.
         * @param serverId      - output parameter, holds the serverId buffer to be populated.
         * @param serverIdSize  - input parameter, size of serverId buffer in bytes.
         *                      - output paramerter, holds the actual size written to serverId buffer.
         *                        if response equals MP_USER_INSUFFICIENT_MEM or if serverId buffer is NULL, holds the pending serverId size.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_USER_INSUFFICIENT_MEM
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult getRegistrationServerInfo(uint16_t &flags, std::string &serverAddress ,uint8_t *serverId, uint16_t &serverIdSize);

        /**
         * Sets the registration server information.
         * Server information can be changed as long as registration not completed and SGX is disabled.
         * To change the server information after registration is completed, the BIOS must be reset to clear the platform registration data.
         *
         * @param flags         - input parameter, holds the registration flags.
         *                        If BIT[0] is set, the registration agent will not send the registration request to the registration server.
         *                        The user will have to read the request and send it to the registration server.
         * @param serverAddress - input parameter, holds the registration server address.
         * @param serverId      - input parameter, holds the serverId buffer to be set.
         * @param serverIdSize  - input parameter, size of serverId buffer in bytes.
         *
         * @return status code, one of:
         *      - MP_SUCCESS
         *      - MP_INVALID_PARAMETER
         *      - MP_UEFI_INTERNAL_ERROR
         */
        MPUefiDllExport MpResult setRegistrationServerInfo(const uint16_t &flags, const std::string &serverAddress ,const uint8_t *serverId, const uint16_t &serverIdSize);

    private:            
        std::unique_ptr<IUefi> m_uefi;
        LogLevel m_logLevel;

        MPUefi& operator=(const MPUefi&) {return *this;}
        MPUefi(const MPUefi& src) {(void) src; }

};

#endif  // #ifndef __MP_UEFI_H
