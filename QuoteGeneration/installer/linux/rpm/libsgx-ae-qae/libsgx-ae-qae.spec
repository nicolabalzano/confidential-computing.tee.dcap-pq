#
# Copyright(c) 2011-2026 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#

%define _license_file COPYING

Name:           libsgx-ae-qae
Version:        @version@
Release:        1%{?dist}
Summary:        Intel(R) Software Guard Extensions QAE
Group:          Development/Libraries

License:        BSD License
URL:            https://github.com/intel/confidential-computing.tee.dcap
Source0:        %{name}-%{version}.tar.gz

AutoProv:       no

%description
Intel(R) Software Guard Extensions QAE

%prep
%setup -qc

%install
make DESTDIR=%{?buildroot} install
install -d %{?buildroot}%{_docdir}/%{name}
find %{?_sourcedir}/package/licenses/ -type f -print0 | xargs -0 -n1 cat >> %{?buildroot}%{_docdir}/%{name}/%{_license_file}
rm -f %{_specdir}/list-%{name}
for f in $(find %{?buildroot} -type f -o -type l); do
    echo $f | sed -e "s#%{?buildroot}##" >> %{_specdir}/list-%{name}
done

%files -f %{_specdir}/list-%{name}

%changelog
* Fri Jan 8 2026 SGX Team
- Initial Release
