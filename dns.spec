Name:	dns	
Version:	0.0.1
Release:	1%{?dist}
Summary:	dns

Group:		-
License:	Apache 2
URL:		https://github.com/b17v134/dns
Source0:	%{name}-%{version}.tar.gz

BuildRequires:	check-devel

%description
dns


%package -n python3-dns
Summary: python interface to DNS C library
BuildRequires: python3-devel python3-pip python3-wheel
Requires: dns

%description -n python3-dns
python interface to DNS C library

%prep
%setup -q


%build
%configure \
    --enable-python

make %{?_smp_mflags}


%install
%make_install


%files
%defattr(-,root,root,-)
%license COPYING
%doc AUTHORS ChangeLog README* NEWS
%{_bindir}/*
%{_includedir}/*
%{_libdir}/*

	
%files -n python3-dns
%defattr(-,root,root,-)
%license COPYING
%doc AUTHORS ChangeLog README* NEWS
%{python3_sitearch}/dns/
%{python3_sitearch}/*.so
%{python3_sitearch}/*.dist-info

%changelog
* Sat Oct 22 2022 b17v134 b17v134@users.noreply.github.com
- First version
