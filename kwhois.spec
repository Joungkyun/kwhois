Summary: Internet whois/nicname client.
Summary(ko): whois/nicname Ŭ���̾�Ʈ ���α׷�
Name: kwhois
Version: 2.3
Release: 1
Copyright: GPL
Group: Applications/Internet
Url: http://www.oops.org/
Source0: ftp://ftp.oops.kr.net/pub/distrib/kwhois/kwhois-%{version}.tar.bz2
Buildroot: /var/tmp/kwhois-root
Conflicts: fwhois, whois

Packager: JoungKyun Kim <http://www.oops.org>
Vendor: OOPS Development ORG

%description
A whois client that accepts both traditional and finger-style queries.

%description -l ko
��Ŀ ��Ÿ�ϰ� �������� ����� �����ϴ� ������ Ŭ���̾�Ʈ ���α׷�

%prep
%setup -q

%build
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -fr $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT{%{_bindir},%{_mandir}/{man1,ko/man1}}
%{makeinstall}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/whois
%{_bindir}/kwhois
%{_mandir}/man1/whois.1*
%{_mandir}/man1/kwhois.1*
%{_mandir}/ko/man1/whois.1.kr*
%{_mandir}/ko/man1/kwhois.1.kr*

%changelog
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
