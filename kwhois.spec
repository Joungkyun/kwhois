%define _unpackaged_files_terminate_build 0
%define svnid $Id: kwhois.spec 121 2014-07-23 08:00:33Z oops $

Summary: Internet whois/nicname client.
Summary(ko): whois/nicname 클라이언트 프로그램
Name: kwhois
Version: 4.4
Release: 1
Epoch: 8
License: GPL
Group: Applications/Internet
Url: http://oops.org/
Source0: ftp://mirror.oops.org/pub/oops/kwhois/kwhois-%{version}.tar.bz2
Buildroot: /var/tmp/kwhois-root
Conflicts: fwhois, whois

Packager: JoungKyun.Kim <http://oops.org>
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
%{__make} %{?_smp_mflags}

%install
[ "%{buildroot}" != "/" ] && %{__rm} -fr %{buildroot}
%{__mkdir_p} %{buildroot}{%{_bindir},%{_mandir}/{man1,ko/man1}}
make DESTDIR=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/*
%{_mandir}/man1/*.1*
%{_mandir}/ko/man1/*.1*
%{_datadir}/locale/*/LC_MESSAGES/kwhois.mo

%changelog
* Wed Jul 23 2014 JoungKyun.Kim <http://oops.org> 8:4.3-1
- release 4.3
- fixed wrong check 2depth domain (ex co.kr)

* Fri Jul 18 2014 JoungKyun.Kim <http://oops.org> 8:4.2-1
- update 4.2

* Wed Jul 16 2014 JoungKyun.Kim <http://oops.org> 8:4.1-1
- update 4.1

* Tue Apr 22 2014 JoungKyun.Kim <http://oops.org> 8:4.0-1
- update 4.0

* Mon Jan  2 2013 JoungKyun.Kim <http://oops.org> 8:3.9-1
- update 3.9

* Thu Nov 22 2012 JoungKyun.Kim <http://oops.org> 8:3.8-1
- update 3.8
- support krnic whois renewal (change charset to utf-8)

* Wed Apr 18 2012 JoungKyun.Kim <http://oops.org> 8:3.7-1
- update 3.7

* Thu Sep 15 2011 JoungKyun.Kim <http://oops.org> 7:3.6-1
- update 3.6

* Wed Jul  9 2008 JoungKyun.Kim <http://oops.org> 6:3.5-1
- update 3.5

* Tue Aug 10 2004 JoungKyun.Kim <http://oops.org> 5:3.4-1
- update 3.4

* Thu Jun  3 2004 JoungKyun.Kim <http://oops.org> 4:3.3-3
- rebuilt

* Thu Jun  3 2004 JoungKyun.Kim <http://oops.org> 3:3.3-2
- fixed extension query format for crsnic

* Thu Jun  3 2004 JoungKyun.Kim <http://oops.org> 2:3.3-1
- update 3.3
- changed query format for miss search domain
- fixed some compile warning message
- fixed library miss link
- added default server per extensions

* Thu Feb 11 2004 JoungKyun.Kim <http://oops.org> 3.2-1
- update 3.2
- fixed org domain to pir.org
- enabled no case sensitive domain

* Thu Feb  5 2004 JoungKyun.Kim <http://oops.org> 3.1-1
- update 3.1
- changed to use punycode on multibyte com/net domain
- bug fixed raceconv

* Fri Aug 22 2003 JoungKyun.Kim <http://oops.org> 3.0-1
- support multibyte domain
- added punycode converter with punyconv
- added racecode converter with raceconv

* Wed Mar 23 2003 JoungKyun.Kim <http://oops.org> 2.3-1
- fixed segmantfault in case of no tail

* Fri Feb  7 2003 JoungKyun.Kim <http://oops.org> 2.2-1
- changed org registra server to whois.networksolutions.com

* Sun Sep 15 2002 JoungKyun.Kim <http://oops.org> 2.1-1
- enable whole local domain in default
- modified man file

* Thu Sep 13 2002 JoungKyun.Kim <http://oops.org> 2.0-1
- added traditional query type
- added tv, biz, info whois server as default server

* Thu Dec 02 1999 JoungKyun.Kim <http://oops.org>
- updated version 1.2
- fixed Internic whois server to whois.networksolutions.com from rs.internic.net

* Fri Nov 08 1999 JoungKyun.Kim <http://oops.org>
- updated version 1.1
- fixed search Handle ID

* Fri Nov 05 1999 JoungKyun.Kim <http://oops.org>
- rpm version updated 1.0-2
- added ko locale

* Fri Oct 29 1999 JoungKyun.Kim <http://oops.org>
- packaging kwhois-1.0-1

