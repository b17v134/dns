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

%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
%make_install


%files
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog README* COPYING NEWS
%{_bindir}/*
%{_includedir}/*
%{_libdir}/*

%changelog
* Sat Oct 22 2022 b17v134 b17v134@users.noreply.github.com
- First version
