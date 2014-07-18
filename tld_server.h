/*
 * Name server list per domain extensions.
 *
 * $Id$
 */

#ifndef TLD_SERVER_H
#define TLD_SERVER_H

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "whois.verisign-grs.com"
//#define DEFAULT_SERVER "whois.crsnic.net"
#endif

#define LO_SERVER     "whois-servers.net"
#define LU_SERVER     "whois.arin.net"

#define KR_SERVER     "whois.krnic.net"
#define JP_SERVER     "whois.jprs.jp"
#define CN_SERVER     "whois.cnnic.net.cn"

// Non *.whois-servers.net
#define BJ_SERVER     "whois.register.bg"
#define BZ_SERVER     "whois.belizenic.bz"
#define NG_SERVER     "whois.nic.net.ng"
#define SU_SERVER     "whois.tcinet.ru"
#define TC_SERVER     "whois.adamsnames.tc"

#include "new_gtlds.h"

const char * tlds[] = {
	"com", DEFAULT_SERVER,
	"net", DEFAULT_SERVER,
	"org", "whois.pir.org",
	"gov", "whois.nic.gov",
	"asia", "whois.nic.asia",
	"aero", "whois.aero",
	"arpa", "whois.iana.org",
	"biz", "whois.nic.biz",
	"cat", "whois.cat",
	"coop", "whois.nic.coop",
	"edu", "whois.educause.net",
	"info", "whois.afilias.info",
	"int", "whois.iana.org",
	"jobs", "jobswhois.verisign-grs.com",
	"mil", "whois.nic.mil",
	"mobi", "whois.dotmobiregistry.net",
	"museum", "whois.museum",
	"name", "whois.nic.name",
	"pro", "whois.nic.pro",
	"tel", "whois.nic.tel",
	"trabel", "whois.nic.travel",
	"xxx", "whois.nic.xxx",
	"xn--3e0b707e", "whois.kr",                  // Korea, Republic of
	"xn--45brj9c", "whois.inregistry.net",       // India, Bengali AW
	"xn--80ao21a", "whois.nic.kz",               // Kazakhstan
	"xn--90a3ac", "whois.rnids.rs",              // Serbia
	"xn--clchc0ea0b2g2a9gcd", "whois.sgnic.sg",  // Singapore, Tamil
	"xn--fiqs8s", "cwhois.cnnic.cn",             // China, Simplified Chinese
	"xn--fiqz9s", "cwhois.cnnic.cn",             // China, Traditional Chinese
	"xn--fpcrj9c3d", "whois.inregistry.net",     // India, Telugu AW
	"xn--fzc2c9e2c", "whois.nic.lk",             // Sri Lanka, Sinhala
	"xn--gecrj9c", "whois.inregistry.net",       // India, Gujarati AW
	"xn--h2brj9c", "whois.inregistry.net",       // India, Hindi AW
	"xn--j1amh", "whois.dotukr.com",             // Ukraine
	"xn--j6w193g", "whois.hkirc.hk",             // Hong Kong
	"xn--kprw13d", "whois.twnic.net.tw",         // Taiwan, Simplified Chinese
	"xn--kpry57d", "whois.twnic.net.tw",         // Taiwan, Traditional Chinese
	"xn--l1acc", "whois.nic.mn",                 // Mongolia AW
	"xn--lgbbat1ad8j", "whois.nic.dz",           // Algeria
	"xn--mgb9awbf", "whois.registry.om",         // Oman AW
	"xn--mgba3a4f16a", "whois.nic.ir",           // Iran
	"xn--mgbaam7a8h", "whois.aeda.net.ae",       // United Arab Emirates
	"xn--mgbbh1a71e", "whois.inregistry.net",    // India, Urdu AW
	"xn--mgberp4a5d4ar", "whois.nic.net.sa",     // Saudi Arabia
	"xn--mgbx4cd0ab", "whois.domainregistry.my", // Malaysia AW
	"xn--o3cw4h", "whois.thnic.co.th",           // Thailand
	"xn--ogbpf8fl", "whois.tld.sy",              // Syria
	"xn--p1ai", "whois.tcinet.ru",               // Russian Federation
	"xn--s9brj9c", "whois.inregistry.net",       // India, Punjabi AW
	"xn--wgbh1c", "whois.dotmasr.eg",            // Egypt
	"xn--wgbl6a", "whois.registry.qa",           // Qatar
	"xn--xkc2al3hye2a", "whois.nic.lk",          // Sri Lanka, Tamil
	"xn--xkc2dl3a5ee0h", "whois.inregistry.net", // India, Tamil AW
	"xn--yfro4i67o", "whois.sgnic.sg",           // Singapore, Chinese
	"xn--ygbi2ammx", "whois.pnina.ps",           // Palestinian Territory
	NULL, NULL
};

const char * two_depth_tlds[] = {
	".br.com",  "whois.centralnic.net",
	".cn.com",  "whois.centralnic.net",
	".de.com",  "whois.centralnic.net",
	".eu.com",  "whois.centralnic.net",
	".gb.com",  "whois.centralnic.net",
	".gb.net",  "whois.centralnic.net",
	".gr.com",  "whois.centralnic.net",
	".hu.com",  "whois.centralnic.net",
	".in.net",  "whois.centralnic.net",
	".no.com",  "whois.centralnic.net",
	".qc.com",  "whois.centralnic.net",
	".ru.com",  "whois.centralnic.net",
	".sa.com",  "whois.centralnic.net",
	".se.com",  "whois.centralnic.net",
	".se.net",  "whois.centralnic.net",
	".uk.com",  "whois.centralnic.net",
	".uk.net",  "whois.centralnic.net",
	".us.com",  "whois.centralnic.net",
	".uy.com",  "whois.centralnic.net",
	".za.com",  "whois.centralnic.net",
	".jpn.com", "whois.centralnic.net",
	".web.com", "whois.centralnic.net",
	".za.net",  "whois.za.net",
	".eu.org",  "whois.eu.org",
	".za.org",  "whois.za.org",
	".e164.arpa",   "whois.ripe.net",
	".priv.at", "whois.nic.priv.at",
	".co.ca",   "whois.co.ca",
	".edu.cn",  "whois.edu.cn",
	".uk.co",   "whois.uk.co",
	".co.pl",   "whois.co.pl",
	".co.nl",   "whois.co.nl",
	".edu.ru",  "whois.informika.ru",
	".biz.ua",  "whois.biz.ua",
	".co.ua",   "whois.co.ua",
	".pp.ua",   "whois.pp.ua",
	".ac.uk",   "whois.ja.net",
	".gov.uk",  "whois.ja.net",
	".fed.us",  "whois.nic.gov",
	".ac.za",   "whois.ac.za",
	".alt.za",  "whois.alt.za",
	".co.za",   "whois.registry.net.za",
	".gov.za",  "whois.gov.za",
	".net.za",  "whois.net.za",
	".web.za",  "whois.web.za",
	NULL
};

#endif
