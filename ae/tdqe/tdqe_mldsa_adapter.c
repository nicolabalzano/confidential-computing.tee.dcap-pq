#include <string.h>

#include "mldsa_native.h"
#include "quoting_enclave_tdqe.h"

int tdqe_mldsa65_keygen(uint8_t *public_key, uint8_t *private_key, uint8_t *seed)
{
    if ((NULL == public_key) || (NULL == private_key) || (NULL == seed)) {
        return -1;
    }

    return MLD_API_NAMESPACE(keypair_internal)(public_key, private_key, seed);
}

int tdqe_mldsa65_sign(uint8_t *signature,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *private_key)
{
    uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES] = {0};
    size_t pre_len = 0;
    size_t signature_len = 0;
    uint8_t deterministic_rnd[MLDSA65_RNDBYTES] = {0};
    int ret = -1;

    if ((NULL == signature) || (NULL == message) || (NULL == private_key)) {
        return -1;
    }

    pre_len = MLD_API_NAMESPACE(prepare_domain_separation_prefix)(pre,
        NULL,
        0,
        NULL,
        0,
        MLD_PREHASH_NONE);
    if (0 == pre_len) {
        return -1;
    }

    ret = MLD_API_NAMESPACE(signature_internal)(signature,
        &signature_len,
        message,
        message_len,
        pre,
        pre_len,
        deterministic_rnd,
        private_key,
        0);
    if ((0 != ret) || (signature_len != SGX_QL_MLDSA_65_SIG_SIZE)) {
        return -1;
    }

    return 0;
}

int tdqe_mldsa65_verify(const uint8_t *signature,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *public_key)
{
    uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES] = {0};
    size_t pre_len = 0;

    if ((NULL == signature) || (NULL == message) || (NULL == public_key)) {
        return -1;
    }

    pre_len = MLD_API_NAMESPACE(prepare_domain_separation_prefix)(pre,
        NULL,
        0,
        NULL,
        0,
        MLD_PREHASH_NONE);
    if (0 == pre_len) {
        return -1;
    }

    return MLD_API_NAMESPACE(verify_internal)(signature,
        SGX_QL_MLDSA_65_SIG_SIZE,
        message,
        message_len,
        pre,
        pre_len,
        public_key,
        0);
}
