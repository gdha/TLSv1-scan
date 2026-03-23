%global debug_package %{nil}

Name:           TLSv1-scan
Version:        1.0
Release:        1%{?dist}
Summary:        Scan local system for services using deprecated TLSv1.0 or TLSv1.1

License:        GPLv3
URL:            https://github.com/gdha/TLSv1-scan
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  rubygem-ronn

Requires:       sslscan
Requires:       iproute
Requires:       lsof
Requires:       net-tools

%description
TLSv1-scan scans all local TCP listening ports and checks each one for
TLSv1.0 or TLSv1.1 support using sslscan, then reports the owning process
for any service where those deprecated protocol versions are enabled.

%prep
%setup -q

%build
%make_build
%make_build man

%install
install -D -m 755 %{name} %{buildroot}%{_sbindir}/%{name}
install -D -m 644 %{name}.8 %{buildroot}%{_mandir}/man8/%{name}.8

%files
%license LICENSE
%doc README.md
%{_sbindir}/%{name}
%{_mandir}/man8/%{name}.8*

%changelog
* Mon Mar 23 2026 Gratien D'haese <gratien.dhaese@gmail.com> - 1.0-1
- Initial RPM package release
