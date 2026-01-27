#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

%define _license_file COPYING

Name:           libsgx-dcap-ql
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions Data Center Attestation Primitives
Group:          Development/Libraries
Requires:       libsgx-qe3-logic >= %{version}-%{release} libsgx-pce-logic >= %{version}-%{release}
Recommends:     libsgx-dcap-quote-verify >= %{version}-%{release} libsgx-quote-ex >= 2.28

License:        BSD License
URL:            https://github.com/intel/SGXDataCenterAttestationPrimitives
Source0:        %{name}-%{version}.tar.gz

%description
Intel(R) Software Guard Extensions Data Center Attestation Primitives

%package devel
Summary:        Intel(R) Software Guard Extensions Data Center Attestation Primitives for Developers
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release} libsgx-headers >= 2.28

%description devel
Intel(R) Software Guard Extensions Data Center Attestation Primitives for Developers

%prep
%setup -qc

%install
make DESTDIR=%{?buildroot} install
install -d %{?buildroot}/%{name}%{_docdir}/%{name}
find %{?_sourcedir}/package/licenses/ -type f -print0 | xargs -0 -n1 cat >> %{?buildroot}/%{name}%{_docdir}/%{name}/%{_license_file}
rm -f %{_specdir}/list-%{name}
for f in $(find %{?buildroot}/%{name} -type f -o -type l); do
    echo $f | sed -e "s#%{?buildroot}/%{name}##" >> %{_specdir}/list-%{name}
done
cp -r %{?buildroot}/%{name}/* %{?buildroot}/
rm -fr %{?buildroot}/%{name}
rm -f %{_specdir}/list-%{name}-devel
for f in $(find %{?buildroot}/%{name}-dev -type f -o -type l); do
    echo $f | sed -e "s#%{?buildroot}/%{name}-dev##" >> %{_specdir}/list-%{name}-devel
done
cp -r %{?buildroot}/%{name}-dev/* %{?buildroot}/
rm -fr %{?buildroot}/%{name}-dev

%files -f %{_specdir}/list-%{name}

%files devel -f %{_specdir}/list-%{name}-devel

%debug_package

%changelog
* Mon Mar 09 2020 SGX Team
- Initial Release
