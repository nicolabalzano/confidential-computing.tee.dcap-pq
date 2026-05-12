/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/*
 * Multi-level SCU wrapper used by the local TDX/TDQE integration.
 *
 * This translation unit carries the shared parameter-set-independent code
 * plus the ML-DSA-65-specific symbols.
 */

#define MLD_CONFIG_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA
#define MLD_CONFIG_MULTILEVEL_WITH_SHARED
#define MLD_CONFIG_PARAMETER_SET 65
#include "mldsa-native/mldsa/mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_CONFIG_MULTILEVEL_WITH_SHARED
#undef MLD_CONFIG_NAMESPACE_PREFIX
