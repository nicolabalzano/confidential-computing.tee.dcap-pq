/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/*
 * Multi-level SCU wrapper used by the local TDX/TDQE integration.
 *
 * This translation unit carries only the ML-DSA-44-specific symbols and
 * relies on the shared base emitted by mldsa_native_65.c.
 */

#define MLD_CONFIG_NAMESPACE_PREFIX PQCP_MLDSA_NATIVE_MLDSA
#define MLD_CONFIG_MULTILEVEL_NO_SHARED
#define MLD_CONFIG_PARAMETER_SET 44
#include "mldsa-native/mldsa/mldsa_native.c"
#undef MLD_CONFIG_PARAMETER_SET
#undef MLD_CONFIG_MULTILEVEL_NO_SHARED
#undef MLD_CONFIG_NAMESPACE_PREFIX
