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
LINUX_INSTALLER_COMMON_AE_QAE_DIR="${LINUX_INSTALLER_COMMON_DIR}/libsgx-ae-qae"

source ${LINUX_INSTALLER_COMMON_AE_QAE_DIR}/installConfig
DEB_FOLDER=${AE_QAE_PACKAGE_NAME}-${AE_QAE_VERSION}

SGX_VERSION=$(awk '/STRFILEVER/ {print $3}' ${ROOT_DIR}/common/inc/internal/se_version.h|sed 's/^\"\(.*\)\"$/\1/')
DEB_BUILD_FOLDER=${AE_QAE_PACKAGE_NAME}-${SGX_VERSION}

main() {
    pre_build
    create_upstream_tarball
    unpack_upstream_tarball
    generate_copyright
    update_version
    rename_tarball
    build_deb_package
    post_build
}

pre_build() {
    rm -fR ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
    cp -fR ${SCRIPT_DIR}/${DEB_FOLDER} ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
}

post_build() {
    rm -fR ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
}

create_upstream_tarball() {
    ${LINUX_INSTALLER_COMMON_AE_QAE_DIR}/createTarball.sh
    cp ${LINUX_INSTALLER_COMMON_AE_QAE_DIR}/output/${TARBALL_NAME} ${SCRIPT_DIR}
}

unpack_upstream_tarball() {
    pushd ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
    cp ../${TARBALL_NAME} .
    tar xvf ${TARBALL_NAME}
    rm -f ${TARBALL_NAME}
    popd
}

generate_copyright() {
    pushd ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
    rm -f debian/copyright
    find package/licenses/ -type f -print0 | xargs -0 -n1 cat >> debian/copyright
    popd
}

get_os_code() {
    OS_CODE=$(lsb_release -cs 2> /dev/null)
    if [ -z ${OS_CODE} ]; then
        OS_CODE=$(grep "VERSION_CODENAME" /etc/os-release 2> /dev/null | cut -d= -f2)
    fi
    echo ${OS_CODE}
}

update_version() {
    pushd ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
    INS_VERSION=$(echo $(dpkg-parsechangelog |grep "Version" | cut -d: -f2))
    DEB_VERSION=$(echo $INS_VERSION | cut -d- -f2)

    FULL_VERSION=${SGX_VERSION}-$(get_os_code)${DEB_VERSION}
    sed -i "s/${INS_VERSION}/${FULL_VERSION}/" debian/changelog
    popd
}

rename_tarball() {
    TARBALL_NAME_NEW_VERSION=$(echo ${TARBALL_NAME} | sed "s/${AE_QAE_VERSION}/${SGX_VERSION}/")
    mv ${SCRIPT_DIR}/${TARBALL_NAME} ${SCRIPT_DIR}/${TARBALL_NAME_NEW_VERSION}
}

build_deb_package() {
    pushd ${SCRIPT_DIR}/${DEB_BUILD_FOLDER}
    SOURCE_DATE_EPOCH="$(date +%s)" dpkg-buildpackage -us -uc
    popd
}

main $@
