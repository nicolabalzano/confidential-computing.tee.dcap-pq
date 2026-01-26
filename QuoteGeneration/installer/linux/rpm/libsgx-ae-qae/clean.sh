#!/usr/bin/env bash
#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#


set -e

SCRIPT_DIR=$(dirname "$0")
COMMON_DIR="${SCRIPT_DIR}/../../common/libsgx-ae-qae"

rm -f ${SCRIPT_DIR}/libsgx*.rpm
rm -f ${COMMON_DIR}/gen_source.py
rm -rf ${COMMON_DIR}/output
