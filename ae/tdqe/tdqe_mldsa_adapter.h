#ifndef _TDQE_MLDSA_ADAPTER_H_
#define _TDQE_MLDSA_ADAPTER_H_

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

int tdqe_mldsa65_keygen(uint8_t *public_key, uint8_t *private_key, uint8_t *seed);
int tdqe_mldsa65_sign(uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *private_key);
int tdqe_mldsa65_verify(const uint8_t *signature, const uint8_t *message, size_t message_len, const uint8_t *public_key);

#if defined(__cplusplus)
}
#endif

#endif
