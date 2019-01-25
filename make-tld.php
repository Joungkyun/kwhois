#!/usr/bin/php
<?php

#
# Generate tdl_server.h and new_gtdls.h from
# https://raw.githubusercontent.com/weppos/whois/master/data/tld.json
#

function whois_server ($v) {
	switch ($v) {
		case 'whois.iana.org' :
			return 'DEFAULT_SERVER';
			break;
		case 'whois.verisign-grs.com' :
			return 'VERISIGN_SERVER';
			break;
		case 'whois.arin.net' :
			return 'LU_SERVER';
			break;
		case 'whois.kr' :
		case 'whois.krnic.net' :
		case 'whois.kisa.or.kr' :
			return 'KR_SERVER';
			break;
		case 'whois.jprs.jp' :
			return 'JP_SERVER';
			break;
		case 'whois.cnnic.net.cn' :
			return 'CN_SERVER';
			break;
	}

	return '"' . $v . '"';
}

ini_set ('allow_url_fopen', true);

if ( ! file_exists ('tldserver.tmp') ) {
	$buf = file_get_contents ('https://raw.githubusercontent.com/weppos/whois/master/data/tld.json');
	file_put_contents ('tldserver.tmp', $buf);
} else {
	$buf = file_get_contents ('tldserver.tmp');
}

$jbuf = json_decode ($buf, false, 512, JSON_PRETTY_PRINT|JSON_UNESCAPED_UNICODE);

#echo print_r ($jbuf) . "\n";

$TOP_levels = [ 'com', 'net', 'org', 'mil', 'gov' ];

foreach ($jbuf as $key => $objs ) {
	if ( $key == '_' )
		continue;

	if ( in_array ($key, $TOP_levels) === true )
		$top_levels[$key] = whois_server ($objs->host);
	else if ( strlen ($key) == 2 )
		$cc_tlds[$key] = whois_server ($objs->host);
	else if ( $objs->host == 'whois.nic.' . $key && $objs->_type == 'newgtld' )
		$new_gtlds[$key] = whois_server ($objs->host);
	else if ( ! isset ($objs->host) )
		$nohosts[$key] = whois_server ($objs->host);
	else if ( preg_replace ('/[^.]/', '', $key) == '.' )
		$two_depths[$key] = whois_server ($objs->host);
	else
		$gtlds[$key] = whois_server ($objs->host);
}

$top_levels['mil'] = whois_server ('whois.nic.mil');

#print_r ($top_levels); 1
#print_r ($new_gtlds);
#print_r ($nohosts);
#print_r ($two_depths); 1
#print_r ($cc_tlds);
#print_r ($gtlds); 1


$buf_tlds = <<<EOL
/*
 * Name server list per domain extensions.
 *
 */

#ifndef TLD_SERVER_H
#define TLD_SERVER_H

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "whois.iana.org"
#endif

#define VERISIGN_SERVER "whois.verisign-grs.com"

#define CC_SERVER     "whois-servers.net"
#define LU_SERVER     "whois.arin.net"

#define KR_SERVER     "whois.kr"
#define JP_SERVER     "whois.jprs.jp"
#define CN_SERVER     "whois.cnnic.net.cn"

// Non *.whois-servers.net
#define BJ_SERVER     "whois.register.bg"
#define BZ_SERVER     "whois.belizenic.bz"
#define NG_SERVER     "whois.nic.net.ng"
#define SU_SERVER     "whois.tcinet.ru"
#define TC_SERVER     "whois.adamsnames.tc"

const char * tlds[] = {
	// original gtlds
	"com",                      VERISIGN_SERVER,
	"net",                      VERISIGN_SERVER,
	"org",                      {$top_levels['org']},
	"gov",                      {$top_levels['gov']},
	"mil",                      {$top_levels['mil']},

	// new gtlds who has un-regular whois server format

EOL;

ksort ($gtlds);
foreach ( $gtlds as $ext => $wserver ) {

	$eext = "\"{$ext}\",";

	if ( strncmp ($ext, "xn--", 4) == 0 )
		$cmt = ' // ' . idn_to_utf8 ($ext);
	else
		$cmt = '';

	$buf_tlds1 = sprintf ('	%-27s %s,', $eext, $wserver);

	if ( strlen ($cmt) )
		$buf_tlds .= sprintf ('%-64s%s', $buf_tlds1, $cmt);
	else
		$buf_tlds .= sprintf ('%s', $buf_tlds1);
	$buf_tlds .="\n";
}

$buf_tlds .= "\n\t// no whois server lists\n";

foreach ( $nohosts as $ext => $wserver ) {
	$eext = "\"{$ext}\",";
	$buf_tlds .= sprintf ('	%-27s %s,', $eext, 'DEFAULT_SERVER');
	$buf_tlds .= "\n";
}

$buf_tlds .= <<<EOL
	NULL, NULL
};

const char * two_depth_tlds[] = {

EOL;

ksort ($two_depths);
foreach ( $two_depths as $ext => $wserver ) {
	$eext = "\".{$ext}\",";
	$buf_tlds .= sprintf ('	%-27s %s,', $eext, $wserver);
	$buf_tlds .="\n";
}


$buf_tlds .= <<<EOL
	NULL, NULL
};

const char * cc_tlds[] = {

EOL;


ksort ($cc_tlds);
foreach ( $cc_tlds as $ext => $wserver ) {
	if ( $wserver == '""' ) {
		$ip = gethostbyname ($ext . '.whois-servers.net' . '.');
		if ( $ip != $ext . '.whois-servers.net' . '.' && $ip !== false )
			$wserver = '"' . $ext . '.whois-servers.net"';
		else {
			$ip = gethostbyname ('whois.nic.' . $ext . '.');
			if ( $ip != 'whois.nic.' . $ext . '.' && $ip !== false )
				$wserver = '"whois.nic.' . $ext . '"';
		}
	}

	if ( $wserver == '""' )
		$wserver = 'DEFAULT_SERVER';

	$eext = "\"{$ext}\",";
	$buf_tlds .= sprintf ('	%-27s %s,', $eext, $wserver);
	$buf_tlds .="\n";
}


$buf_tlds .= <<<EOL
	NULL, NULL
};

#include "new_gtlds.h"

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

EOL;

file_put_contents ('tld_server.h.new', $buf_tlds);



$buf_tlds = <<<EOL
/*
 * Name server list per new gtld extensions.
 *
 */

const char * new_tglds[] = {

EOL;

foreach ($new_gtlds as $ext => $wserver) {
	$buf_tlds .= "\t\"{$ext}\",\n";
}

$buf_tlds .= <<<EOL
	NULL
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

EOL;

echo $buf_tlds . "\n";

file_put_contents ('new_gtlds.h.new', $buf_tlds);

?>
