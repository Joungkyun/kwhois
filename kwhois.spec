Summary: Internet whois/nicname client.
Summary(ko): whois/nicname 클라이언트 프로그램
Name: kwhois
Version: 3.3
Release: 1
Epoch: 2
Copyright: GPL
Group: Applications/Internet
Url: http://www.oops.org/
Source0: ftp://mirror.oops.org/pub/Linux/OOPS/Source/kwhois/kwhois-%{version}.tar.bz2
Buildroot: /var/tmp/kwhois-root
Conflicts: fwhois, whois

Packager: JoungKyun Kim <http://www.oops.org>
Vendor: OOPS Development ORG

%description
A whois client that accepts both traditional and finger-style queries.
This support to search multibyte domain based on punycode

%description -l ko
핑커 스타일과 전통적인 방식을 지원하는 후이즈 클라이언트 프로그램
kwhois 는 punycode 를 이용한 다국어 도메인을 지원한다.

%prep
%setup -q

%build
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -fr $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT{%{_bindir},%{_mandir}/{man1,ko/man1}}
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/whois
%{_bindir}/kwhois
%{_bindir}/punyconv
%{_bindir}/raceconv
%{_mandir}/man1/whois.1*
%{_mandir}/man1/kwhois.1*
%{_mandir}/ko/man1/whois.1*
%{_mandir}/ko/man1/kwhois.1*
%{_datadir}/locale/*/LC_MESSAGES/kwhois.mo

%changelog
* Thu Jun  3 2004 JoungKyun Kim <http://www.oops.org> 2:3.3-1
- changed query format for miss search domain
- fixed some compile warning message
- update 3.3

* Thu Feb 11 2004 JoungKyun Kim <http://www.oops.org> 3.2-1
- update 3.2
- fixed org domain to pir.org
- enabled no case sensitive domain

* Thu Feb  5 2004 JoungKyun Kim <http://www.oops.org> 3.1-1
- update 3.1
- changed to use punycode on multibyte com/net domain
- bug fixed raceconv

* Fri Aug 22 2003 JoungKyun Kim <http://www.oops.org> 3.0-1
- support multibyte domain
- added punycode converter with punyconv
- added racecode converter with raceconv

* Wed Mar 23 2003 JoungKyun Kim <http://www.oops.org> 2.3-1
- fixed segmantfault in case of no tail

* Fri Feb  7 2003 JoungKyun Kim <http://www.oops.org> 2.2-1
- changed org registra server to whois.networksolutions.com

* Sun Sep 15 2002 JoungKyun Kim <http://www.oops.org> 2.1-1
- enable whole local domain in default
- modified man file

* Thu Sep 13 2002 JoungKyun Kim <http://www.oops.org> 2.0-1
- added traditional query type
- added tv, biz, info whois server as default server

* Thu Dec 02 1999 JoungKyun, Kim <http://www.oops.org>
- updated version 1.2
- fixed Internic whois server to whois.networksolutions.com from rs.internic.net

* Fri Nov 08 1999 JoungKyun, Kim <http://www.oops.org>
- updated version 1.1
- fixed search Handle ID

* Fri Nov 05 1999 JoungKyun, Kim <http://www.oops.org>
- rpm version updated 1.0-2
- added ko locale

* Fri Oct 29 1999 JoungKyun, Kim <http://www.oops.org>
- packaging kwhois-1.0-1

