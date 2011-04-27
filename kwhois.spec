Summary: Internet whois/nicname client.
Summary(ko): whois/nicname 클라이언트 프로그램
Name: kwhois
Version: 2.0
Release: 1
Copyright: GPL
Group: Applications/Internet
Url: http://www.oops.org/
Source0: ftp://ftp.oops.kr.net/pub/distrib/kwhois/kwhois-%{version}.tar.gz
Buildroot: /var/tmp/kwhois-root
Conflicts: fwhois, whois

Packager: JoungKyun Kim <http://www.oops.org>
Vendor: OOPS Development ORG

%description
A whois client that accepts both traditional and finger-style queries.

%description -l ko
핑커 스타일과 전통적인 방식을 지원하는 후이즈 클라이언트 프로그램

%prep
%setup

%build
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -fr $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT{%{_bindir},%{_mandir}/man1}
%{makeinstall}

%files
%defattr(-,root,root)
%doc COPYING
%doc docs/{README,README.kr,INSTALL,INSTALL.kr}
%{_bindir}/whois
%{_bindir}/kwhois
%{_mandir}/man1/whois.1*
%{_mandir}/man1/kwhois.1*

%changelog
* Thu Sep 12 2002 JoungKyun Kim <admin@oops.org> 2.0-1
- added traditional query type
- added tv, biz, info whois server as default server

* Thu Dec 02 1999 JoungKyun, Kim <admin@oops.org>
- updated version 1.2
- fixed Internic whois server to whois.networksolutions.com from rs.internic.net

* Fri Nov 08 1999 JoungKyun, Kim <admin@oops.kr.net>
- updated version 1.1
- fixed search Handle ID

* Fri Nov 05 1999 JoungKyun, Kim <admin@oops.kr.net>
- rpm version updated 1.0-2
- added ko locale

* Fri Oct 29 1999 JoungKyun, Kim <admin@oops.kr.net>
- packaging kwhois-1.0-1

