/*
 * Copyright(c) 2011-2026 Intel Corporation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>

#ifdef TDX_TEST_MOCK
// Selective mock switches.
// If not explicitly set, in mock mode we default to mocking both report path
// and wrapper path so the test can run on generic hosts.
#ifndef TDX_TEST_MOCK_REPORT
#define TDX_TEST_MOCK_REPORT 1
#endif
#ifndef TDX_TEST_MOCK_WRAPPER
#define TDX_TEST_MOCK_WRAPPER 1
#endif
#endif

#if defined(TDX_TEST_MOCK_REPORT) || defined(TDX_TEST_MOCK_WRAPPER)
typedef struct _tdx_report_data_t { uint8_t d[64]; } tdx_report_data_t;
typedef struct _tdx_report_t { uint8_t d[1024]; } tdx_report_t;

typedef enum _tdx_attest_error_t {
    TDX_ATTEST_SUCCESS = 0x0000,
    TDX_ATTEST_ERROR_UNEXPECTED = 0x0001,
} tdx_attest_error_t;

typedef enum _tee_att_error_t {
    TEE_ATT_SUCCESS = 0x0000,
    TEE_ATT_ERROR_UNEXPECTED = 0x00011001,
} tee_att_error_t;

typedef struct tee_att_config_t {
    uint32_t tag;
} tee_att_config_t;

typedef struct _mock_quote_t {
    uint32_t magic;         // 'MQT1'
    uint32_t report_size;
    uint32_t signature_size;
    uint32_t report_hash;
    uint8_t signature[64];
    uint8_t report[1024];
} mock_quote_t;

static uint32_t mock_hash_fnv1a(const uint8_t* p_data, size_t size)
{
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < size; ++i) {
        h ^= p_data[i];
        h *= 16777619u;
    }
    return h;
}

static void mock_build_signature(uint32_t report_hash, uint8_t* p_sig, size_t sig_size)
{
    for (size_t i = 0; i < sig_size; ++i) {
        uint8_t b = (uint8_t)((report_hash >> ((i % 4) * 8)) & 0xFF);
        p_sig[i] = (uint8_t)(b ^ (uint8_t)(0x5A + (i & 0x1F)));
    }
}
#endif

#ifdef TDX_TEST_MOCK_REPORT
static tdx_attest_error_t tdx_att_get_report(const tdx_report_data_t *p_tdx_report_data,
                                             tdx_report_t *p_tdx_report)
{
    std::printf("[mock] tdx_att_get_report() called\n");
    if (!p_tdx_report_data || !p_tdx_report) {
        return TDX_ATTEST_ERROR_UNEXPECTED;
    }
    memset(p_tdx_report->d, 0xA5, sizeof(p_tdx_report->d));
    memcpy(p_tdx_report->d, p_tdx_report_data->d,
           sizeof(p_tdx_report_data->d) < sizeof(p_tdx_report->d) ? sizeof(p_tdx_report_data->d) : sizeof(p_tdx_report->d));
    return TDX_ATTEST_SUCCESS;
}
#else
#include "tdx_attest/tdx_attest.h"
#endif

#ifdef TDX_TEST_MOCK_WRAPPER
static tee_att_error_t tee_att_create_context(const void*, const char*, tee_att_config_t** pp_context)
{
    std::printf("[mock] tee_att_create_context() called\n");
    if (!pp_context) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }
    *pp_context = new tee_att_config_t{0x5444584D}; // "TDXM"
    return TEE_ATT_SUCCESS;
}

static tee_att_error_t tee_att_free_context(tee_att_config_t* p_context)
{
    std::printf("[mock] tee_att_free_context() called\n");
    delete p_context;
    return TEE_ATT_SUCCESS;
}

static tee_att_error_t tee_att_init_quote(const tee_att_config_t* p_context,
                                          void*,
                                          bool,
                                          size_t* p_pub_key_id_size,
                                          uint8_t* p_pub_key_id)
{
    std::printf("[mock] tee_att_init_quote() called\n");
    if (!p_context || !p_pub_key_id_size) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }
    if (!p_pub_key_id) {
        *p_pub_key_id_size = 32;
        return TEE_ATT_SUCCESS;
    }
    if (*p_pub_key_id_size < 32) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }
    for (size_t i = 0; i < 32; ++i) {
        p_pub_key_id[i] = (uint8_t)(0xC0 + (i & 0x0F));
    }
    return TEE_ATT_SUCCESS;
}

static tee_att_error_t tee_att_get_quote_size(const tee_att_config_t* p_context,
                                              uint32_t* p_quote_size)
{
    std::printf("[mock] tee_att_get_quote_size() called\n");
    if (!p_context || !p_quote_size) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }
    *p_quote_size = (uint32_t)sizeof(mock_quote_t);
    return TEE_ATT_SUCCESS;
}

static tee_att_error_t tee_att_get_quote(const tee_att_config_t* p_context,
                                         const uint8_t* p_report,
                                         uint32_t report_size,
                                         void*,
                                         uint8_t* p_quote,
                                         uint32_t quote_size)
{
    std::printf("[mock] tee_att_get_quote() called\n");
    if (!p_context || !p_report || !p_quote || report_size == 0) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }

    if (quote_size < sizeof(mock_quote_t) || report_size != sizeof(tdx_report_t)) {
        return TEE_ATT_ERROR_UNEXPECTED;
    }

    mock_quote_t* p_mq = reinterpret_cast<mock_quote_t*>(p_quote);
    memset(p_mq, 0, sizeof(*p_mq));
    p_mq->magic = 0x3154514Du; // 'MQT1'
    p_mq->report_size = report_size;
    p_mq->signature_size = sizeof(p_mq->signature);
    p_mq->report_hash = mock_hash_fnv1a(p_report, report_size);
    mock_build_signature(p_mq->report_hash, p_mq->signature, sizeof(p_mq->signature));
    memcpy(p_mq->report, p_report, report_size);

    return TEE_ATT_SUCCESS;
}

static bool verify_mock_quote_signature(const std::vector<uint8_t>& quote,
                                        const tdx_report_t& td_report)
{
    if (quote.size() < sizeof(mock_quote_t)) {
        std::printf("[test] mock verification failed: quote too small\n");
        return false;
    }

    const mock_quote_t* p_mq = reinterpret_cast<const mock_quote_t*>(quote.data());
    if (p_mq->magic != 0x3154514Du) {
        std::printf("[test] mock verification failed: bad magic\n");
        return false;
    }
    if (p_mq->report_size != sizeof(td_report)) {
        std::printf("[test] mock verification failed: bad report_size=%u\n", p_mq->report_size);
        return false;
    }
    if (p_mq->signature_size != sizeof(p_mq->signature)) {
        std::printf("[test] mock verification failed: bad signature_size=%u\n", p_mq->signature_size);
        return false;
    }

    uint32_t expected_hash = mock_hash_fnv1a(reinterpret_cast<const uint8_t*>(&td_report), sizeof(td_report));
    if (expected_hash != p_mq->report_hash) {
        std::printf("[test] mock verification failed: report hash mismatch\n");
        return false;
    }

    uint8_t expected_sig[64] = {0};
    mock_build_signature(expected_hash, expected_sig, sizeof(expected_sig));
    if (0 != memcmp(expected_sig, p_mq->signature, sizeof(expected_sig))) {
        std::printf("[test] mock verification failed: signature mismatch\n");
        return false;
    }

    if (0 != memcmp(p_mq->report, &td_report, sizeof(td_report))) {
        std::printf("[test] mock verification failed: embedded report mismatch\n");
        return false;
    }

    std::printf("[test] mock verification success: signature and report are consistent\n");
    return true;
}
#else
#include "tdx_quote/inc/td_ql_wrapper.h"
#endif

// Fill the 64-byte report-data buffer with pseudo-random bytes.
// This value is bound into the TD report/quote and can be used by the verifier
// as caller-provided freshness/application context.
static void fill_report_data(tdx_report_data_t *p_report_data)
{
    std::srand((unsigned int)std::time(nullptr));
    for (size_t i = 0; i < sizeof(p_report_data->d); ++i) {
        p_report_data->d[i] = (uint8_t)(std::rand() & 0xFF);
    }
}

int main()
{
#ifdef TDX_TEST_MOCK
    std::printf("[test] running in MOCK mode"
#ifdef TDX_TEST_MOCK_REPORT
                " [report-mock]"
#endif
#ifdef TDX_TEST_MOCK_WRAPPER
                " [wrapper-mock]"
#endif
                "\n");
#endif
    // Input data that will be cryptographically bound to the TD report.
    tdx_report_data_t report_data = {{0}};

    // Raw TD report returned by the TDX guest-attest interface.
    tdx_report_t td_report = {{0}};

    // Context owned by the TDX quote wrapper (lifecycle: create -> use -> free).
    tee_att_config_t *p_ctx = nullptr;

    // Return codes from the two APIs used in this test:
    // - tdx_attest (guest side report retrieval)
    // - td_ql_wrapper (quote wrapper flow)
    tee_att_error_t tee_ret = TEE_ATT_SUCCESS;
    tdx_attest_error_t attest_ret = TDX_ATTEST_SUCCESS;

    // Step 1) Prepare report-data payload.
    fill_report_data(&report_data);

    // Step 2) Ask the TDX guest interface for a TD report.
    // The TD report will later be converted to a signed quote.
    std::printf("[test] request TD report...\n");
    attest_ret = tdx_att_get_report(&report_data, &td_report);
    if (TDX_ATTEST_SUCCESS != attest_ret) {
        std::printf("[test] tdx_att_get_report failed: 0x%x\n", attest_ret);
        return 1;
    }

    // Step 3) Create quote-wrapper context with default attestation key identity.
    std::printf("[test] create TDX quote context...\n");
    tee_ret = tee_att_create_context(nullptr, nullptr, &p_ctx);
    if (TEE_ATT_SUCCESS != tee_ret || nullptr == p_ctx) {
        std::printf("[test] tee_att_create_context failed: 0x%x\n", tee_ret);
        return 1;
    }

    // Step 4) Query required public-key-id buffer size.
    // Passing p_pub_key_id == nullptr is the official size-query pattern.
    size_t pub_key_id_size = 0;
    tee_ret = tee_att_init_quote(p_ctx, nullptr, false, &pub_key_id_size, nullptr);
    if (TEE_ATT_SUCCESS != tee_ret || 0 == pub_key_id_size) {
        std::printf("[test] tee_att_init_quote(size query) failed: 0x%x, size=%zu\n", tee_ret, pub_key_id_size);
        tee_att_free_context(p_ctx);
        return 1;
    }

    // Step 5) Retrieve the actual public-key-id of the selected attestation key.
    std::vector<uint8_t> pub_key_id(pub_key_id_size, 0);
    tee_ret = tee_att_init_quote(p_ctx, nullptr, false, &pub_key_id_size, pub_key_id.data());
    if (TEE_ATT_SUCCESS != tee_ret) {
        std::printf("[test] tee_att_init_quote failed: 0x%x\n", tee_ret);
        tee_att_free_context(p_ctx);
        return 1;
    }

    // Step 6) Ask wrapper for required quote buffer size.
    uint32_t quote_size = 0;
    tee_ret = tee_att_get_quote_size(p_ctx, &quote_size);
    if (TEE_ATT_SUCCESS != tee_ret || 0 == quote_size) {
        std::printf("[test] tee_att_get_quote_size failed: 0x%x, quote_size=%u\n", tee_ret, quote_size);
        tee_att_free_context(p_ctx);
        return 1;
    }

    // Step 7) Generate quote from the TD report.
    // This is the call that triggers debug prints added around quote generation.
    std::vector<uint8_t> quote(quote_size, 0);
    std::printf("[test] generate quote (this should trigger [tdx-quote-debug] prints)...\n");
    tee_ret = tee_att_get_quote(
        p_ctx,
        reinterpret_cast<const uint8_t *>(&td_report),
        (uint32_t)sizeof(td_report),
        nullptr,
        quote.data(),
        quote_size);

    if (TEE_ATT_SUCCESS != tee_ret) {
        std::printf("[test] tee_att_get_quote failed: 0x%x\n", tee_ret);
        tee_att_free_context(p_ctx);
        return 1;
    }

#ifdef TDX_TEST_MOCK_WRAPPER
    if (!verify_mock_quote_signature(quote, td_report)) {
        tee_att_free_context(p_ctx);
        return 1;
    }
#endif

    std::printf("[test] quote generated successfully: %u bytes\n", quote_size);

    // Step 8) Clean up wrapper context.
    tee_att_free_context(p_ctx);
    return 0;
}
