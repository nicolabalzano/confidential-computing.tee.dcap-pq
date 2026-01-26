#!/usr/bin/env bash
#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

set -e

SCRIPT_DIR=$(dirname "$0")
COMMON_DIR="${SCRIPT_DIR}/../../common/libsgx-ae-qae"

rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.deb
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.ddeb
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.tar.gz
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.tar.xz
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.dsc
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.changes
rm -f ${SCRIPT_DIR}/libsgx-ae-qae*.buildinfo
rm -f ${COMMON_DIR}/gen_source.py
rm -rf ${COMMON_DIR}/output
