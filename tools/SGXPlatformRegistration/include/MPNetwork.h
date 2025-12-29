/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * File: MPNetwork.h 
 *   
 * Description: Class definition for the network functionality to 
 * communicate with the Registration Sever. 
 */
#ifndef MPNETWORK_H_
#define MPNETWORK_H_

#include "MPNetworkDefs.h"
#include <string>
using std::string;

class IMPSynchronicSender;

#ifdef _WIN32
#define MPNetworkDllExport  __declspec(dllexport)
#else
#define MPNetworkDllExport
#endif

/**
 * This is the main entry point for the SGX Multi-Package Network CPP interface.
 * Used to send requests to the SGX Registration Server.
 */

class MPNetwork {
public:
    /**
     * MPNetwork class constructor
     *
     * @param serverAddress     - input parameter, server address.
     * @param subscriptionKey   - input parameter, server subscription key.
     * @param proxy             - input parameter, desired proxy configurations.
     * @param logLevel          - input parameter, desired logging level.
     */
    MPNetworkDllExport MPNetwork(const string serverAddress, const string subscriptionKey, const ProxyConf &proxy, const LogLevel logLevel = MP_REG_LOG_LEVEL_ERROR);

    /**
     * Sends request to the registration server.
     * 
     * @param requestType       - input parameter, request type.
     * @param request           - input parameter, binary request buffer to be sent.
     * @param requestSize       - input parameter, size of request buffer in bytes.
     * @param response          - optional input parameter, binary response buffer to be populated.
     * @param responseSize      - input parameter, size of response buffer in bytes.
     *                          - output paramerter, holds the actual size written to response buffer.
     *                            if result equals to MP_USER_INSUFFICIENT_MEM, holds the pending response size.
     * @param statusCode        - output parameter, holds the received HTTP status code.
     * @param errorCode         - output parameter, if statusCode equals to MPA_RS_BAD_REQUEST, holds the response error code.
     *
     * @return status code, one of:
     *      - MP_SUCCESS
     *      - MP_INVALID_PARAMETER
     *      - MP_USER_INSUFFICIENT_MEM
     *      - MP_NETWORK_ERROR
     *      - MP_UNEXPECTED_ERROR
     */
    MPNetworkDllExport MpResult sendBinaryRequest(const MpRequestType &requestType, const uint8_t *request, const uint32_t &requestSize,
        uint8_t *response, uint16_t &responseSize, HttpStatusCode &statusCode, RegistrationErrorCode &errorCode);

    /**
     * MPNetwork class destructor
     */
    MPNetworkDllExport virtual ~MPNetwork();
private:
    IMPSynchronicSender* m_sender;
    string m_serverAddress;
    string m_subscriptionKey;
    LogLevel m_logLevel;
    MPNetwork& operator=(const MPNetwork&) {return *this;}
    MPNetwork(const MPNetwork& src) {(void) src; }

};

#endif // #ifndef MPNETWORK_H_
