/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */
/** File: network_wrapper.h 
 *  
 * Description: Definitions of network access interfaces
 *
 */

#ifndef NETWORK_WRAPPER_H_
#define NETWORK_WRAPPER_H_

#include <cstdint>

typedef enum  _cache_server_delivery_status_t {
    DELIVERY_SUCCESS = 0,
    DELIVERY_FAIL,
    DELIVERY_ERROR_MAX = 0xff,
} cache_server_delivery_status_t;

typedef enum  _network_post_error_t {
    POST_SUCCESS = 0,
    POST_UNEXPECTED_ERROR,
    POST_INVALID_PARAMETER_ERROR,
    POST_OUT_OF_MEMORY_ERROR,
    POST_AUTHENTICATION_ERROR,
    POST_NETWORK_ERROR
} network_post_error_t;

network_post_error_t network_https_post(const uint8_t* raw_data, const uint64_t raw_data_size, const uint16_t platform_id_length, const bool non_enclave_mode);

bool is_server_url_available();

#endif /* !NETWORK_WRAPPER_H_ */

