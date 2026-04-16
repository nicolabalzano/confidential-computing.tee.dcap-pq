#include <string.h>

#include "mldsa_native.h"
#include "quoting_enclave_tdqe.h"

typedef int (*mldsa_keygen_t)(uint8_t *, uint8_t *, const uint8_t *);
typedef int (*mldsa_sign_t)(uint8_t *, size_t *, const uint8_t *, size_t, const uint8_t *, size_t, const uint8_t *, const uint8_t *, int);
typedef int (*mldsa_verify_t)(const uint8_t *, size_t, const uint8_t *, size_t, const uint8_t *, size_t, const uint8_t *, int);
typedef size_t (*mldsa_prepare_prefix_t)(uint8_t[MLD_DOMAIN_SEPARATION_MAX_BYTES], const uint8_t *, size_t, const uint8_t *, size_t, int);

int PQCP_MLDSA_NATIVE_MLDSA87_keypair_internal(uint8_t *public_key, uint8_t *private_key, const uint8_t *seed);
int PQCP_MLDSA_NATIVE_MLDSA87_signature_internal(uint8_t *signature, size_t *signature_len, const uint8_t *message, size_t message_len, const uint8_t *prefix, size_t prefix_len, const uint8_t *deterministic_rnd, const uint8_t *private_key, int context);
int PQCP_MLDSA_NATIVE_MLDSA87_verify_internal(const uint8_t *signature, size_t signature_len, const uint8_t *message, size_t message_len, const uint8_t *prefix, size_t prefix_len, const uint8_t *public_key, int context);
size_t PQCP_MLDSA_NATIVE_MLDSA87_prepare_domain_separation_prefix(uint8_t prefix[MLD_DOMAIN_SEPARATION_MAX_BYTES], const uint8_t *ph, size_t phlen, const uint8_t *ctx, size_t ctxlen, int hashalg);

typedef struct _tdqe_mldsa_backend_t {
    mldsa_keygen_t keygen;
    mldsa_sign_t sign;
    mldsa_verify_t verify;
    mldsa_prepare_prefix_t prepare_prefix;
    size_t signature_size;
    size_t rnd_size;
} tdqe_mldsa_backend_t;

static const tdqe_mldsa_backend_t *get_mldsa_backend(uint32_t algorithm_id)
{
    static const tdqe_mldsa_backend_t mldsa65_backend = {
        PQCP_MLDSA_NATIVE_MLDSA65_keypair_internal,
        PQCP_MLDSA_NATIVE_MLDSA65_signature_internal,
        PQCP_MLDSA_NATIVE_MLDSA65_verify_internal,
        PQCP_MLDSA_NATIVE_MLDSA65_prepare_domain_separation_prefix,
        SGX_QL_MLDSA_65_SIG_SIZE,
        MLDSA65_RNDBYTES
    };
    static const tdqe_mldsa_backend_t mldsa87_backend = {
        PQCP_MLDSA_NATIVE_MLDSA87_keypair_internal,
        PQCP_MLDSA_NATIVE_MLDSA87_signature_internal,
        PQCP_MLDSA_NATIVE_MLDSA87_verify_internal,
        PQCP_MLDSA_NATIVE_MLDSA87_prepare_domain_separation_prefix,
        SGX_QL_MLDSA_87_SIG_SIZE,
        MLDSA87_RNDBYTES
    };

    switch (algorithm_id) {
    case SGX_QL_ALG_MLDSA_65:
        return &mldsa65_backend;
    case SGX_QL_ALG_MLDSA_87:
        return &mldsa87_backend;
    default:
        return NULL;
    }
}

static int tdqe_mldsa_keygen(uint32_t algorithm_id, uint8_t *public_key, uint8_t *private_key, const uint8_t *seed)
{
    const tdqe_mldsa_backend_t *backend = get_mldsa_backend(algorithm_id);

    if ((NULL == public_key) || (NULL == private_key) || (NULL == seed)) {
        return -1;
    }
    if (backend == NULL) {
        return -1;
    }

    return backend->keygen(public_key, private_key, seed);
}

static int tdqe_mldsa_sign(uint32_t algorithm_id,
    uint8_t *signature,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *private_key)
{
    const tdqe_mldsa_backend_t *backend = get_mldsa_backend(algorithm_id);
    uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES] = {0};
    size_t pre_len = 0;
    size_t signature_len = 0;
    uint8_t deterministic_rnd[MLDSA87_RNDBYTES] = {0};
    int ret = -1;

    if ((NULL == signature) || (NULL == message) || (NULL == private_key)) {
        return -1;
    }
    if (backend == NULL) {
        return -1;
    }

    pre_len = backend->prepare_prefix(pre,
        NULL,
        0,
        NULL,
        0,
        MLD_PREHASH_NONE);
    if (0 == pre_len) {
        return -1;
    }

    ret = backend->sign(signature,
        &signature_len,
        message,
        message_len,
        pre,
        pre_len,
        deterministic_rnd,
        private_key,
        0);
    if ((0 != ret) || (signature_len != backend->signature_size)) {
        return -1;
    }

    return 0;
}

static int tdqe_mldsa_verify(uint32_t algorithm_id,
    const uint8_t *signature,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *public_key)
{
    const tdqe_mldsa_backend_t *backend = get_mldsa_backend(algorithm_id);
    uint8_t pre[MLD_DOMAIN_SEPARATION_MAX_BYTES] = {0};
    size_t pre_len = 0;

    if ((NULL == signature) || (NULL == message) || (NULL == public_key)) {
        return -1;
    }
    if (backend == NULL) {
        return -1;
    }

    pre_len = backend->prepare_prefix(pre,
        NULL,
        0,
        NULL,
        0,
        MLD_PREHASH_NONE);
    if (0 == pre_len) {
        return -1;
    }

    return backend->verify(signature,
        backend->signature_size,
        message,
        message_len,
        pre,
        pre_len,
        public_key,
        0);
}

int tdqe_mldsa65_keygen(uint8_t *public_key, uint8_t *private_key, uint8_t *seed)
{
    return tdqe_mldsa_keygen(SGX_QL_ALG_MLDSA_65, public_key, private_key, seed);
}

int tdqe_mldsa65_sign(uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *private_key)
{
    return tdqe_mldsa_sign(SGX_QL_ALG_MLDSA_65, signature, message, message_len, private_key);
}

int tdqe_mldsa65_verify(const uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *public_key)
{
    return tdqe_mldsa_verify(SGX_QL_ALG_MLDSA_65, signature, message, message_len, public_key);
}

int tdqe_mldsa87_keygen(uint8_t *public_key, uint8_t *private_key, uint8_t *seed)
{
    return tdqe_mldsa_keygen(SGX_QL_ALG_MLDSA_87, public_key, private_key, seed);
}

int tdqe_mldsa87_sign(uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *private_key)
{
    return tdqe_mldsa_sign(SGX_QL_ALG_MLDSA_87, signature, message, message_len, private_key);
}

int tdqe_mldsa87_verify(const uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *public_key)
{
    return tdqe_mldsa_verify(SGX_QL_ALG_MLDSA_87, signature, message, message_len, public_key);
}
