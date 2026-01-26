#!/usr/bin/env bash
#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#


set -e

SCRIPT_DIR=$(dirname "$0")
ROOT_DIR="${SCRIPT_DIR}/../../../../"
LINUX_INSTALLER_DIR="${ROOT_DIR}/installer/linux"
LINUX_INSTALLER_COMMON_DIR="${LINUX_INSTALLER_DIR}/common"

INSTALL_PATH=${SCRIPT_DIR}/output

# Cleanup
rm -fr ${INSTALL_PATH}

# Get the configuration for this package
source ${SCRIPT_DIR}/installConfig

# Fetch the gen_source script
cp ${LINUX_INSTALLER_COMMON_DIR}/gen_source/gen_source.py ${SCRIPT_DIR}

# Copy the files according to the BOM
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-ae-qae.txt
python ${SCRIPT_DIR}/gen_source.py --bom=BOMs/libsgx-ae-qae-package.txt  --cleanup=false
python ${SCRIPT_DIR}/gen_source.py --bom=../licenses/BOM_license.txt --cleanup=false

# Create the tarball
SGX_VERSION=$(awk '/QAE_VERSION/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
pushd ${INSTALL_PATH} &> /dev/null
sed -i "s/USR_LIB_VER=.*/USR_LIB_VER=${SGX_VERSION}/" Makefile
tar -zcvf ${TARBALL_NAME} *
popd &> /dev/null
