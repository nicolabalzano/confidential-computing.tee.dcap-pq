/*
 * Copyright (C) 2011-2022 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "qgs_ql_logic.h"
#include "qgs_log.h"
#include "qgs_msg_lib.h"
#include "se_trace.h"
#include "sgx_ql_lib_common.h"
#include "td_ql_wrapper.h"
#include "tdx_attest.h"
#include <boost/thread.hpp>
#include <boost/thread/detail/thread.hpp>
#include <boost/thread/tss.hpp>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>

static const sgx_ql_att_key_id_t k_qgs_default_ecdsa_p256_att_key_id =
{
    0,
    0,
    32,
    { 0x8c, 0x4f, 0x57, 0x75, 0xd7, 0x96, 0x50, 0x3e, 0x96, 0x13, 0x7f, 0x77, 0xc6, 0x8a, 0x82, 0x9a,
      0x00, 0x56, 0xac, 0x8d, 0xed, 0x70, 0x14, 0x0b, 0x08, 0x1b, 0x09, 0x44, 0x90, 0xc5, 0x7b, 0xff,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    2,
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    SGX_QL_ALG_ECDSA_P256
};

static const sgx_ql_att_key_id_t k_qgs_default_mldsa_65_att_key_id =
{
    0,
    0,
    32,
    { 0x8c, 0x4f, 0x57, 0x75, 0xd7, 0x96, 0x50, 0x3e, 0x96, 0x13, 0x7f, 0x77, 0xc6, 0x8a, 0x82, 0x9a,
      0x00, 0x56, 0xac, 0x8d, 0xed, 0x70, 0x14, 0x0b, 0x08, 0x1b, 0x09, 0x44, 0x90, 0xc5, 0x7b, 0xff,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    2,
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    SGX_QL_ALG_MLDSA_65
};

typedef quote3_error_t (*get_collateral_func)(const uint8_t *fmspc,
                                              uint16_t fmspc_size, const char *pck_ca,
                                              tdx_ql_qv_collateral_t **pp_quote_collateral);
typedef quote3_error_t (*free_collateral_func)(tdx_ql_qv_collateral_t *p_quote_collateral);
typedef quote3_error_t (*sgx_ql_set_logging_callback_t)(sgx_ql_logging_callback_t logger,
                                                        sgx_ql_log_level_t loglevel);

void sgx_ql_logging_callback(sgx_ql_log_level_t level, const char *message) {
    if (level == SGX_QL_LOG_ERROR) {
        sgx_proc_log_report(1, "%s", message);

    } else if (level == SGX_QL_LOG_INFO) {
        sgx_proc_log_report(3, "%s", message);
    }
}

void cleanup(tee_att_config_t *p_ctx) {
    QGS_LOG_INFO("About to delete ctx in cleanup\n");
    tee_att_free_context(p_ctx);
    return;
}

boost::thread_specific_ptr<tee_att_config_t> ptr(cleanup);

namespace intel { namespace sgx { namespace dcap { namespace qgs {

    static bool is_tdx_uuid_equal(const tdx_uuid_t& lhs, const uint8_t *rhs)
    {
        return rhs != NULL && std::memcmp(lhs.d, rhs, sizeof(lhs.d)) == 0;
    }

    static bool select_tdx_att_key_id_from_uuid_list(const uint8_t *p_id_list,
                                                     uint32_t id_list_size,
                                                     tee_att_att_key_id_t *p_selected_key_id,
                                                     tdx_uuid_t *p_selected_uuid)
    {
        static const tdx_uuid_t kEcdsaUuid = {TDX_SGX_ECDSA_ATTESTATION_ID};
        static const tdx_uuid_t kMldsaUuid = {TDX_SGX_MLDSA_65_ATTESTATION_ID};
        const uint32_t uuid_size = sizeof(tdx_uuid_t);

        if (p_selected_key_id == NULL || p_selected_uuid == NULL) {
            return false;
        }

        std::memset(p_selected_key_id, 0, sizeof(*p_selected_key_id));
        std::memset(p_selected_uuid, 0, sizeof(*p_selected_uuid));

        if (p_id_list == NULL || id_list_size == 0) {
            return false;
        }

        if ((id_list_size % uuid_size) != 0) {
            return false;
        }

        for (uint32_t offset = 0; offset < id_list_size; offset += uuid_size) {
            const uint8_t *candidate = p_id_list + offset;
            if (is_tdx_uuid_equal(kMldsaUuid, candidate)) {
                if (std::memcpy(&p_selected_key_id->base,
                                &k_qgs_default_mldsa_65_att_key_id,
                                sizeof(k_qgs_default_mldsa_65_att_key_id)) == NULL) {
                    return false;
                }
                *p_selected_uuid = kMldsaUuid;
                return true;
            }
            if (is_tdx_uuid_equal(kEcdsaUuid, candidate)) {
                if (std::memcpy(&p_selected_key_id->base,
                                &k_qgs_default_ecdsa_p256_att_key_id,
                                sizeof(k_qgs_default_ecdsa_p256_att_key_id)) == NULL) {
                    return false;
                }
                *p_selected_uuid = kEcdsaUuid;
                return true;
            }
        }
        return false;
    }

    static tee_att_error_t ensure_context_for_quote_request(const uint8_t *p_id_list,
                                                            uint32_t id_list_size,
                                                            tdx_uuid_t *p_selected_uuid)
    {
        static const tdx_uuid_t kDefaultUuid = {TDX_SGX_ECDSA_ATTESTATION_ID};
        tee_att_att_key_id_t current_key_id = {};
        tee_att_att_key_id_t requested_key_id = {};
        tdx_uuid_t requested_uuid = {};
        tee_att_error_t ret = TEE_ATT_SUCCESS;

        if (ptr.get() == 0) {
            if (p_selected_uuid) {
                *p_selected_uuid = kDefaultUuid;
            }
            return TEE_ATT_SUCCESS;
        }

        if (!select_tdx_att_key_id_from_uuid_list(p_id_list, id_list_size, &requested_key_id, &requested_uuid)) {
            if (p_selected_uuid) {
                *p_selected_uuid = kDefaultUuid;
            }
            return TEE_ATT_SUCCESS;
        }
        fprintf(stderr, "[qgs-debug] ensure_context requested algorithm_id=%u\n", requested_key_id.base.algorithm_id);
        fflush(stderr);

        ret = tee_att_get_keyid(ptr.get(), &current_key_id);
        if (ret != TEE_ATT_SUCCESS) {
            fprintf(stderr, "[qgs-debug] ensure_context tee_att_get_keyid ret=0x%x\n", ret);
            fflush(stderr);
            return ret;
        }
        fprintf(stderr, "[qgs-debug] ensure_context current algorithm_id=%u\n", current_key_id.base.algorithm_id);
        fflush(stderr);

        if (current_key_id.base.algorithm_id == requested_key_id.base.algorithm_id) {
            if (p_selected_uuid) {
                *p_selected_uuid = requested_uuid;
            }
            return TEE_ATT_SUCCESS;
        }

        ptr.reset(nullptr);

        tee_att_config_t *p_ctx = NULL;
        fprintf(stderr, "[qgs-debug] ensure_context about to create selected context algorithm_id=%u\n",
                requested_key_id.base.algorithm_id);
        fflush(stderr);
        ret = tee_att_create_context(&requested_key_id, NULL, &p_ctx);
        fprintf(stderr, "[qgs-debug] ensure_context create_context ret=0x%x ctx=%p\n", ret, (void*)p_ctx);
        fflush(stderr);
        if (ret != TEE_ATT_SUCCESS) {
            return ret;
        }
        ptr.reset(p_ctx);
        if (p_selected_uuid) {
            *p_selected_uuid = requested_uuid;
        }
        return TEE_ATT_SUCCESS;
    }

    // Function to check if any byte within [start, end) in a vector is non-zero
    bool is_any_byte_none_zero(const uint8_t* p, size_t size) {
        // Use std::any_of to check if any element in the specified range is non-zero
        return std::any_of(p, p + size,
                           [](uint8_t value)
                           { return value != 0; });
    }

    data_buffer get_resp(const uint8_t *p_req, uint32_t req_size) {
        fprintf(stderr, "[qgs-debug] get_resp enter req_size=%u\n", req_size);
        fflush(stderr);

        tee_att_error_t tee_att_ret = TEE_ATT_SUCCESS;
        qgs_msg_error_t qgs_msg_error_ret = QGS_MSG_SUCCESS;
        uint8_t *p_resp = NULL;
        uint32_t resp_size = 0;
        uint32_t resp_error_code = QGS_MSG_ERROR_UNEXPECTED;

        uint32_t req_type = QGS_MSG_TYPE_MAX;
        if (QGS_MSG_SUCCESS != qgs_msg_get_type(p_req, req_size, &req_type)) {
            QGS_LOG_ERROR("Cannot get msg type\n");
            fprintf(stderr, "[qgs-debug] qgs_msg_get_type failed\n");
            fflush(stderr);
            return {};
        }
        fprintf(stderr, "[qgs-debug] get_resp req_type=%u\n", req_type);
        fflush(stderr);
        if (ptr.get() == 0) {
            tee_att_error_t ret = TEE_ATT_SUCCESS;
            tee_att_config_t *p_ctx = NULL;
            QGS_LOG_INFO("call tee_att_create_context\n");
            fprintf(stderr, "[qgs-debug] about to call tee_att_create_context\n");
            fflush(stderr);
            ret = tee_att_create_context(NULL, NULL, &p_ctx);
            fprintf(stderr, "[qgs-debug] tee_att_create_context ret=0x%x ctx=%p\n", ret, (void*)p_ctx);
            fflush(stderr);
            if (TEE_ATT_SUCCESS != ret) {
                QGS_LOG_ERROR("Cannot create context\n");
                return {};
            }
            std::ostringstream oss;
            oss << boost::this_thread::get_id();
            QGS_LOG_INFO("create context in thread[%s]\n", oss.str().c_str());
            ptr.reset(p_ctx);

            do {
                void *p_handle = NULL;
                tee_att_ret = ::tee_att_get_qpl_handle(ptr.get(), &p_handle);
                if (TEE_ATT_SUCCESS != tee_att_ret || NULL == p_handle) {
                    QGS_LOG_WARN("tee_att_get_qpl_handle return 0x%x\n", tee_att_ret);
                    break;
                }

                sgx_ql_set_logging_callback_t ql_set_logging_callback =
                    (sgx_ql_set_logging_callback_t)dlsym(p_handle, "sgx_ql_set_logging_callback");
                if (dlerror() == NULL && ql_set_logging_callback) {
                    ql_set_logging_callback(sgx_ql_logging_callback,
                                            qgs_debug ? SGX_QL_LOG_INFO : SGX_QL_LOG_ERROR);
                } else {
                    QGS_LOG_WARN("Failed to set logging callback for the quote provider library.\n");
                }
            } while(0);

            if (req_type != GET_PLATFORM_INFO_REQ && req_type != GET_QUOTE_REQ) {
                sgx_target_info_t qe_target_info;
                uint8_t hash[32] = {0};
                size_t hash_size = sizeof(hash);
                fprintf(stderr, "[qgs-debug] about to call tee_att_init_quote bootstrap\n");
                fflush(stderr);
                tee_att_ret = tee_att_init_quote(ptr.get(), &qe_target_info, false, &hash_size, hash);
                fprintf(stderr, "[qgs-debug] tee_att_init_quote bootstrap ret=0x%x hash_size=%zu\n",
                        tee_att_ret, hash_size);
                fflush(stderr);
                if (TEE_ATT_SUCCESS != tee_att_ret) {
                    QGS_LOG_ERROR("tee_att_init_quote return 0x%x\n", tee_att_ret);
                    return {};
                } else {
                    QGS_LOG_INFO("tee_att_init_quote return success\n");
                }
            }
        }

        switch (req_type) {
        case GET_QUOTE_REQ: {
            uint32_t size = 0;

            const uint8_t *p_report;
            uint32_t report_size;
            const uint8_t *p_id_list;
            uint32_t id_list_size;
            tdx_uuid_t selected_tdx_uuid = {TDX_SGX_ECDSA_ATTESTATION_ID};

            data_buffer quote_buf;

            qgs_msg_error_ret = qgs_msg_inflate_get_quote_req(p_req,
                                                        req_size,
                                                        &p_report, &report_size,
                                                        &p_id_list, &id_list_size);
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret) {
                // TODO: need to define the error code list for R3AAL
                resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                QGS_LOG_ERROR("qgs_msg_inflate_get_quote_req return error\n");
            } else {
                QGS_LOG_INFO("GET_QUOTE_REQ: report_size=%u id_list_size=%u\n", report_size, id_list_size);
                tee_att_ret = ensure_context_for_quote_request(p_id_list, id_list_size, &selected_tdx_uuid);
                fprintf(stderr, "[qgs-debug] ensure_context_for_quote_request ret=0x%x\n", tee_att_ret);
                fflush(stderr);
                if (tee_att_ret != TEE_ATT_SUCCESS) {
                    resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                    QGS_LOG_ERROR("ensure_context_for_quote_request return 0x%x\n", tee_att_ret);
                    break;
                }
                QGS_LOG_INFO("GET_QUOTE_REQ: selected uuid=%02x%02x%02x%02x... algorithm-aware context ready\n",
                             selected_tdx_uuid.d[0], selected_tdx_uuid.d[1],
                             selected_tdx_uuid.d[2], selected_tdx_uuid.d[3]);
                {
                    sgx_target_info_t qe_target_info = {};
                    uint8_t hash[32] = {0};
                    size_t hash_size = sizeof(hash);
                    fprintf(stderr, "[qgs-debug] GET_QUOTE_REQ about to call tee_att_init_quote selected-context bootstrap\n");
                    fflush(stderr);
                    tee_att_ret = tee_att_init_quote(ptr.get(), &qe_target_info, false, &hash_size, hash);
                    fprintf(stderr, "[qgs-debug] GET_QUOTE_REQ tee_att_init_quote selected-context ret=0x%x hash_size=%zu\n",
                            tee_att_ret, hash_size);
                    fflush(stderr);
                    if (TEE_ATT_SUCCESS != tee_att_ret) {
                        resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                        QGS_LOG_ERROR("GET_QUOTE_REQ selected-context tee_att_init_quote return 0x%x\n", tee_att_ret);
                        break;
                    }
                }
                int retry = 1;

                do {
                    if (retry == 0) {
                        sgx_target_info_t qe_target_info;
                        uint8_t hash[32] = {0};
                        size_t hash_size = sizeof(hash);
                        QGS_LOG_INFO("call tee_att_init_quote\n");
                        tee_att_ret = tee_att_init_quote(ptr.get(), &qe_target_info, true,
                                                        &hash_size,
                                                        hash);
                        if (TEE_ATT_SUCCESS != tee_att_ret) {
                            resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                            QGS_LOG_ERROR("tee_att_init_quote return 0x%x\n", tee_att_ret);
                        } else {
                            QGS_LOG_INFO("tee_att_init_quote return Success\n");
                        }
                    }
                    QGS_LOG_INFO("GET_QUOTE_REQ: calling tee_att_get_quote_size\n");
                    fprintf(stderr, "[qgs-debug] about to call tee_att_get_quote_size\n");
                    fflush(stderr);
                    if (TEE_ATT_SUCCESS != (tee_att_ret = tee_att_get_quote_size(ptr.get(), &size))) {
                        resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                        QGS_LOG_ERROR("tee_att_get_quote_size return 0x%x\n", tee_att_ret);
                    } else {
                        QGS_LOG_INFO("tee_att_get_quote_size return Success, size=%u\n", size);
                        fprintf(stderr, "[qgs-debug] tee_att_get_quote_size ret=0x%x size=%u\n", tee_att_ret, size);
                        fflush(stderr);
                        quote_buf.resize(size);
                        QGS_LOG_INFO("GET_QUOTE_REQ: calling tee_att_get_quote\n");
                        fprintf(stderr, "[qgs-debug] about to call tee_att_get_quote\n");
                        fflush(stderr);
                        tee_att_ret = tee_att_get_quote(ptr.get(),
                                                        p_report,
                                                        report_size,
                                                        NULL,
                                                        quote_buf.data(),
                                                        size);
                        fprintf(stderr, "[qgs-debug] tee_att_get_quote ret=0x%x\n", tee_att_ret);
                        fflush(stderr);
                        if (TEE_ATT_SUCCESS != tee_att_ret) {
                            resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                            QGS_LOG_ERROR("tee_att_get_quote return 0x%x\n", tee_att_ret);
                        } else {
                            resp_error_code = QGS_MSG_SUCCESS;
                            QGS_LOG_INFO("tee_att_get_quote return Success\n");
                        }
                    }
                // Only retry once when the return code is TEE_ATT_ATT_KEY_NOT_INITIALIZED
                } while (TEE_ATT_ATT_KEY_NOT_INITIALIZED == tee_att_ret && retry--);
            }
            if (resp_error_code == QGS_MSG_SUCCESS) {
                QGS_LOG_INFO("GET_QUOTE_REQ: generating success response size=%u\n", size);
                qgs_msg_error_ret = qgs_msg_gen_get_quote_resp(selected_tdx_uuid.d,
                                                               sizeof(selected_tdx_uuid.d),
                                                               quote_buf.data(),
                                                               size,
                                                               &p_resp,
                                                               &resp_size);
            } else {
                QGS_LOG_ERROR("GET_QUOTE_REQ: generating error response code=%u\n", resp_error_code);
                qgs_msg_error_ret = qgs_msg_gen_error_resp(resp_error_code, GET_QUOTE_RESP, &p_resp, &resp_size);
            }
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret) {
                QGS_LOG_ERROR("call qgs_msg_gen function failed\n");
                qgs_msg_free(p_resp);
                return {};
            }
            break;
        }
        case GET_COLLATERAL_REQ: {
            const uint8_t *p_fsmpc;
            uint32_t fsmpc_size;
            const uint8_t *p_pckca;
            uint32_t pckca_size;
            tdx_ql_qv_collateral_t *p_collateral = NULL;
            free_collateral_func free_func = NULL;

            qgs_msg_error_ret = qgs_msg_inflate_get_collateral_req(p_req,
                                                            req_size,
                                                            &p_fsmpc, &fsmpc_size,
                                                            &p_pckca, &pckca_size);
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret || fsmpc_size >= UINT16_MAX) {
                resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                QGS_LOG_ERROR("qgs_msg_inflate_get_collateral_req return error\n");
            } else {
                do {
                    char *error1 = NULL;
                    char *error2 = NULL;
                    void *p_handle = NULL;
                    quote3_error_t quote3_ret = SGX_QL_SUCCESS;
                    tee_att_ret = ::tee_att_get_qpl_handle(ptr.get(), &p_handle);
                    if (TEE_ATT_SUCCESS != tee_att_ret || NULL == p_handle) {
                        resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                        QGS_LOG_ERROR("tee_att_get_qpl_handle return 0x%x\n", tee_att_ret);
                        break;
                    }

                    auto get_func = (get_collateral_func)dlsym(p_handle, "tdx_ql_get_quote_verification_collateral");
                    error1 = dlerror();
                    free_func = (free_collateral_func)dlsym(p_handle, "tdx_ql_free_quote_verification_collateral");
                    error2 = dlerror();
                    if ((NULL == error1) && (NULL != get_func) && (NULL == error2) && (NULL != free_func)) {
                        SE_PROD_LOG("Found tdx quote verification functions.\n");
                        quote3_ret = get_func(p_fsmpc, (uint16_t)fsmpc_size, (const char *)p_pckca, &p_collateral);
                        if (SGX_QL_SUCCESS != quote3_ret) {
                            resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                            QGS_LOG_ERROR("tdx_ql_get_quote_verification_collateral return %d\n", quote3_ret);
                            break;
                        } else {
                            resp_error_code = QGS_MSG_SUCCESS;
                            QGS_LOG_INFO("tdx_ql_get_quote_verification_collateral return SUCCESS\n");
                            break;
                        }
                    } else {
                        resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                        QGS_LOG_ERROR("Cannot find tdx quote verification functions.\n");
                        break;
                    }
                } while (0);
            }
            if (resp_error_code == QGS_MSG_SUCCESS) {
                qgs_msg_error_ret = qgs_msg_gen_get_collateral_resp(p_collateral->major_version, p_collateral->minor_version,
                                                                    (const uint8_t *)p_collateral->pck_crl_issuer_chain, p_collateral->pck_crl_issuer_chain_size,
                                                                    (const uint8_t *)p_collateral->root_ca_crl, p_collateral->root_ca_crl_size,
                                                                    (const uint8_t *)p_collateral->pck_crl, p_collateral->pck_crl_size,
                                                                    (const uint8_t *)p_collateral->tcb_info_issuer_chain, p_collateral->tcb_info_issuer_chain_size,
                                                                    (const uint8_t *)p_collateral->tcb_info, p_collateral->tcb_info_size,
                                                                    (const uint8_t *)p_collateral->qe_identity_issuer_chain, p_collateral->qe_identity_issuer_chain_size,
                                                                    (const uint8_t *)p_collateral->qe_identity, p_collateral->qe_identity_size,
                                                                    &p_resp, &resp_size,
                                                                    (qgs_msg_header_t *)p_req);
                free_func(p_collateral);
            } else {
                qgs_msg_error_ret = qgs_msg_gen_error_resp(resp_error_code, GET_COLLATERAL_RESP, &p_resp, &resp_size);
            }
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret) {
                QGS_LOG_ERROR("call qgs_msg_gen function failed\n");
                qgs_msg_free(p_resp);
                return {};
            }
            break;
        }
        case GET_PLATFORM_INFO_REQ: {
            tee_platform_info_t platform_info;
            qgs_msg_error_ret = qgs_msg_inflate_get_platform_info_req(p_req, req_size);
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret) {
                // TODO: need to define the error code list for R3AAL
                resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                QGS_LOG_ERROR("qgs_msg_inflate_get_platform_info_req return error\n");
            } else {
                QGS_LOG_INFO("call tee_att_get_platform_info\n");
                tee_att_ret = tee_att_get_platform_info(ptr.get(), &platform_info);
                if (TEE_ATT_SUCCESS != tee_att_ret) {
                    resp_error_code = QGS_MSG_ERROR_UNEXPECTED;
                    QGS_LOG_ERROR("tee_att_get_platform_info return 0x%x\n", tee_att_ret);
                } else {
                    resp_error_code = QGS_MSG_SUCCESS;
                    QGS_LOG_INFO("tee_att_get_platform_info return Success\n");
                }
            }
            if (resp_error_code == QGS_MSG_SUCCESS) {
                qgs_msg_error_ret = qgs_msg_gen_get_platform_info_resp(platform_info.tdqe_isv_svn,
                                                                       platform_info.pce_isv_svn,
                                                                       (uint8_t *)&(platform_info.platform_id), sizeof(platform_info.platform_id),
                                                                       (uint8_t *)&(platform_info.cpu_svn), sizeof(platform_info.cpu_svn),
                                                                       &p_resp, &resp_size);
            } else {
                qgs_msg_error_ret = qgs_msg_gen_error_resp(resp_error_code, GET_PLATFORM_INFO_RESP, &p_resp, &resp_size);
            }
            if (QGS_MSG_SUCCESS != qgs_msg_error_ret) {
                QGS_LOG_ERROR("call qgs_msg_gen function failed\n");
                qgs_msg_free(p_resp);
                return {};
            }
            break;
        }
        default:
            QGS_LOG_ERROR("Whoops, bad request!");
            return {};
        }

        QGS_LOG_INFO("Return from get_resp\n");
        data_buffer resp(p_resp, p_resp + resp_size);
        qgs_msg_free(p_resp);
        return resp;
    }

    data_buffer get_raw_resp(const uint8_t *req, uint32_t req_size) {
        tee_att_error_t tee_att_ret = TEE_ATT_SUCCESS;
        data_buffer resp;

        if (ptr.get() == 0) {
            tee_att_error_t ret = TEE_ATT_SUCCESS;
            tee_att_config_t *p_ctx = NULL;
            QGS_LOG_INFO("call tee_att_create_context\n");
            ret = tee_att_create_context(NULL, NULL, &p_ctx);
            if (TEE_ATT_SUCCESS != ret) {
                QGS_LOG_ERROR("Cannot create context\n");
                return {};
            }

            std::ostringstream oss;
            oss << boost::this_thread::get_id();
            QGS_LOG_INFO("create context in thread[%s]\n", oss.str().c_str());
            ptr.reset(p_ctx);

            do {
                void *p_handle = NULL;
                tee_att_ret = ::tee_att_get_qpl_handle(ptr.get(), &p_handle);
                if (TEE_ATT_SUCCESS != tee_att_ret || NULL == p_handle) {
                    QGS_LOG_WARN("tee_att_get_qpl_handle return 0x%x\n", tee_att_ret);
                    break;
                }

                sgx_ql_set_logging_callback_t ql_set_logging_callback =
                    (sgx_ql_set_logging_callback_t)dlsym(p_handle, "sgx_ql_set_logging_callback");
                if (dlerror() == NULL && ql_set_logging_callback) {
                    ql_set_logging_callback(sgx_ql_logging_callback,
                                            qgs_debug ? SGX_QL_LOG_INFO : SGX_QL_LOG_ERROR);
                } else {
                    QGS_LOG_WARN("Failed to set logging callback for the quote provider library.\n");
                }
            } while(0);

            sgx_target_info_t qe_target_info;
            uint8_t hash[32] = {0};
            size_t hash_size = sizeof(hash);
            tee_att_ret = tee_att_init_quote(ptr.get(), &qe_target_info, false,
                    &hash_size,
                    hash);
            if (TEE_ATT_SUCCESS != tee_att_ret) {
                QGS_LOG_ERROR("tee_att_init_quote return 0x%x\n", tee_att_ret);
                //ingnore failure
            } else {
                QGS_LOG_INFO("tee_att_init_quote return success\n");
            }
        }

        if (req_size == sizeof(sgx_report2_t)) {
            sgx_report2_t * p_report = (sgx_report2_t *)req;
            if (p_report->report_mac_struct.report_type.type != TEE_REPORT2_TYPE
                || (p_report->report_mac_struct.report_type.subtype != TEE_REPORT2_SUBTYPE_0
                    && p_report->report_mac_struct.report_type.subtype != TEE_REPORT2_SUBTYPE_1)
                || (p_report->report_mac_struct.report_type.version != TEE_REPORT2_VERSION_0
                    && p_report->report_mac_struct.report_type.version != TEE_REPORT2_VERSION_1
                    && p_report->report_mac_struct.report_type.version != TEE_REPORT2_VERSION_3)
                || p_report->report_mac_struct.report_type.reserved != 0
                || is_any_byte_none_zero(p_report->report_mac_struct.reserved1, SGX_REPORT2_MAC_STRUCT_RESERVED1_BYTES)
                || is_any_byte_none_zero(p_report->report_mac_struct.reserved2, SGX_REPORT2_MAC_STRUCT_RESERVED2_BYTES)
                || is_any_byte_none_zero(p_report->reserved, SGX_REPORT2_RESERVED_BYTES)
                ) {
                QGS_LOG_ERROR("Not a legimit TD report, stop\n");
                return {};
            }

            int retry = 1;
            do {
                uint32_t size = 0;
                if (retry == 0) {
                    sgx_target_info_t qe_target_info;
                    uint8_t hash[32] = {0};
                    size_t hash_size = sizeof(hash);
                    QGS_LOG_INFO("call tee_att_init_quote\n");
                    tee_att_ret = tee_att_init_quote(ptr.get(), &qe_target_info, true,
                                                     &hash_size,
                                                     hash);
                    if (TEE_ATT_SUCCESS != tee_att_ret) {
                        QGS_LOG_ERROR("tee_att_init_quote return 0x%x\n", tee_att_ret);
                    } else {
                        QGS_LOG_INFO("tee_att_init_quote return Success\n");
                    }
                }
                if (TEE_ATT_SUCCESS != (tee_att_ret = tee_att_get_quote_size(ptr.get(), &size))) {
                    QGS_LOG_ERROR("tee_att_get_quote_size return 0x%x\n", tee_att_ret);
                } else {
                    QGS_LOG_INFO("tee_att_get_quote_size return Success\n");
                    resp.resize(size);
                    tee_att_ret = tee_att_get_quote(ptr.get(),
                                                    req,
                                                    req_size,
                                                    NULL,
                                                    resp.data(),
                                                    size);
                    if (TEE_ATT_SUCCESS != tee_att_ret) {
                        resp.resize(0);
                        QGS_LOG_ERROR("tee_att_get_quote return 0x%x\n", tee_att_ret);
                    } else {
                        QGS_LOG_INFO("tee_att_get_quote return Success\n");
                    }
                }
                // Only retry once when the return code is TEE_ATT_ATT_KEY_NOT_INITIALIZED
            } while (TEE_ATT_ATT_KEY_NOT_INITIALIZED == tee_att_ret && retry--);

            return resp;
        } else {
            QGS_LOG_INFO("Not a legimit raw request, stop\n");
            return {};
        }
    }
}
} // namespace dcap
} // namespace sgx
} // namespace intel
