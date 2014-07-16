/*
 * Name server list per domain extensions.
 *
 * $Id$
 */

#ifndef TLD_SERVER_H
#define TLD_SERVER_H

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "whois.crsnic.net"
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
	"com", "whois.networksolutions.com",
	"net", "whois.networksolutions.com",
	"org", "whois.pir.org",
	"gov", "whois.nic.gov"
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
