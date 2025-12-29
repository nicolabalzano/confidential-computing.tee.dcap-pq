#
# Copyright(c) 2011-2025 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

%define _install_path @install_path@

Name:           tdx-qgs
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) TD Quoting Generation Service
Group:          Development/System
Requires:       libsgx-tdx-logic >= %{version}-%{release}

License:        BSD License
URL:            https://github.com/intel/SGXDataCenterAttestationPrimitives
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) TD Quoting Generation Service

%prep
%setup -qc

%build
make %{?_smp_mflags}

%install
make DESTDIR=%{?buildroot} install
echo "%{_install_path}" > %{_specdir}/list-%{name}
find %{?buildroot} | sort | \
awk '$0 !~ last "/" {print last} {last=$0} END {print last}' | \
sed -e "s#^%{?buildroot}##" | \
grep -v "^%{_install_path}" >> %{_specdir}/list-%{name} || :
sed -i 's#^/etc/qgsd.conf#%config &#' %{_specdir}/list-%{name}

%files -f %{_specdir}/list-%{name}

%posttrans
if [ -x %{_install_path}/startup.sh ]; then %{_install_path}/startup.sh; fi

%preun
if [ -x %{_install_path}/cleanup.sh ]; then %{_install_path}/cleanup.sh; fi

%changelog
* Thu Feb 18 2021 SGX Team
- Initial Release
