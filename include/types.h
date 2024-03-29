#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

// https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml#dns-parameters-4
#define DNS_TYPE_ERROR 0
#define DNS_TYPE_A 1
#define DNS_TYPE_NS 2
#define DNS_TYPE_MD 3
#define DNS_TYPE_MF 4
#define DNS_TYPE_CNAME 5
#define DNS_TYPE_SOA 6
#define DNS_TYPE_MB 7
#define DNS_TYPE_MG 8
#define DNS_TYPE_MR 9
#define DNS_TYPE_NULL 10
#define DNS_TYPE_WKS 11
#define DNS_TYPE_PTR 12
#define DNS_TYPE_HINFO 13
#define DNS_TYPE_MINFO 14
#define DNS_TYPE_MX 15
#define DNS_TYPE_TXT 16
#define DNS_TYPE_RP 17
#define DNS_TYPE_AFSDB 18
#define DNS_TYPE_X25 19
#define DNS_TYPE_ISDN 20
#define DNS_TYPE_RT 21
#define DNS_TYPE_NSAP 22
#define DNS_TYPE_NSAP_PTR 23
#define DNS_TYPE_SIG 24
#define DNS_TYPE_KEY 25
#define DNS_TYPE_PX 26
#define DNS_TYPE_GPOS 27
#define DNS_TYPE_AAAA 28
#define DNS_TYPE_LOC 29
#define DNS_TYPE_NXT 30
#define DNS_TYPE_EID 31
#define DNS_TYPE_NIMLOC 32
#define DNS_TYPE_SRV 33
#define DNS_TYPE_ATMA 34
#define DNS_TYPE_NAPTR 35
#define DNS_TYPE_KX 36
#define DNS_TYPE_CERT 37
#define DNS_TYPE_A6 38
#define DNS_TYPE_DNAME 39
#define DNS_TYPE_SINK 40
#define DNS_TYPE_OPT 41
#define DNS_TYPE_APL 42
#define DNS_TYPE_DS 43
#define DNS_TYPE_SSHFP 44
#define DNS_TYPE_IPSECKEY 45
#define DNS_TYPE_RRSIG 46
#define DNS_TYPE_NSEC 47
#define DNS_TYPE_DNSKEY 48
#define DNS_TYPE_DHCID 49
#define DNS_TYPE_NSEC3 50
#define DNS_TYPE_NSEC3PARAM 51
#define DNS_TYPE_TLSA 52
#define DNS_TYPE_SMIMEA 53

#define DNS_TYPE_HIP 55
#define DNS_TYPE_NINFO 56
#define DNS_TYPE_RKEY 57
#define DNS_TYPE_TALINK 58
#define DNS_TYPE_CDS 59
#define DNS_TYPE_CDNSKEY 60
#define DNS_TYPE_OPENPGPKEY 61
#define DNS_TYPE_CSYNC 62
#define DNS_TYPE_ZONEMD 63
#define DNS_TYPE_SVCB 64
#define DNS_TYPE_HTTPS 65

#define DNS_TYPE_SPF 99
#define DNS_TYPE_UINFO 100
#define DNS_TYPE_UID 101
#define DNS_TYPE_GID 102
#define DNS_TYPE_UNSPEC 103
#define DNS_TYPE_NID 104
#define DNS_TYPE_L32 105
#define DNS_TYPE_L64 106
#define DNS_TYPE_LP 107
#define DNS_TYPE_EUI48 108
#define DNS_TYPE_EUI64 109

#define DNS_TYPE_TKEY 249
#define DNS_TYPE_TSIG 250
#define DNS_TYPE_IXFR 251
#define DNS_TYPE_AXFR 252
#define DNS_TYPE_MAILB 253
#define DNS_TYPE_MAILA 254

#define DNS_TYPE_ANY 255

#define DNS_TYPE_URI 256
#define DNS_TYPE_CAA 257
#define DNS_TYPE_AVC 258
#define DNS_TYPE_DOA 259
#define DNS_TYPE_AMTREPLAY 260

#define DNS_TYPE_TA 32768
#define DNS_TYPE_DLV 32769

#define DNS_STR_TYPE_A "A"
#define DNS_STR_TYPE_NS "NS"
#define DNS_STR_TYPE_MD "MD"
#define DNS_STR_TYPE_MF "MF"
#define DNS_STR_TYPE_CNAME "CNAME"
#define DNS_STR_TYPE_SOA "SOA"
#define DNS_STR_TYPE_MB "MB"
#define DNS_STR_TYPE_MG "MG"
#define DNS_STR_TYPE_MR "MR"
#define DNS_STR_TYPE_NULL "NULL"
#define DNS_STR_TYPE_WKS "WKS"
#define DNS_STR_TYPE_PTR "PTR"
#define DNS_STR_TYPE_HINFO "HINFO"
#define DNS_STR_TYPE_MINFO "MINFO"
#define DNS_STR_TYPE_MX "MX"
#define DNS_STR_TYPE_TXT "TXT"
#define DNS_STR_TYPE_RP "RP"
#define DNS_STR_TYPE_AFSDB "AFSDB"
#define DNS_STR_TYPE_X25 "X25"
#define DNS_STR_TYPE_ISDN "ISDN"
#define DNS_STR_TYPE_RT "RT"
#define DNS_STR_TYPE_NSAP "NSAP"
#define DNS_STR_TYPE_NSAP_PTR "NSAP-PTR"
#define DNS_STR_TYPE_SIG "SIG"
#define DNS_STR_TYPE_KEY "KEY"
#define DNS_STR_TYPE_PX "PX"
#define DNS_STR_TYPE_GPOS "GPOS"
#define DNS_STR_TYPE_AAAA "AAAA"
#define DNS_STR_TYPE_LOC "LOC"
#define DNS_STR_TYPE_NXT "NXT"
#define DNS_STR_TYPE_EID "EID"
#define DNS_STR_TYPE_NIMLOC "NIMLOC"
#define DNS_STR_TYPE_SRV "SRV"
#define DNS_STR_TYPE_ATMA "ATMA"
#define DNS_STR_TYPE_NAPTR "NAPTR"
#define DNS_STR_TYPE_KX "KX"
#define DNS_STR_TYPE_CERT "CERT"
#define DNS_STR_TYPE_A6 "A6"
#define DNS_STR_TYPE_DNAME "DNAME"
#define DNS_STR_TYPE_SINK "SINK"
#define DNS_STR_TYPE_OPT "OPT"
#define DNS_STR_TYPE_APL "APL"
#define DNS_STR_TYPE_DS "DS"
#define DNS_STR_TYPE_SSHFP "SSHFP"
#define DNS_STR_TYPE_IPSECKEY "IPSECKEY"
#define DNS_STR_TYPE_RRSIG "RRSIG"
#define DNS_STR_TYPE_NSEC "NSEC"
#define DNS_STR_TYPE_DNSKEY "DNSKEY"
#define DNS_STR_TYPE_DHCID "DHCID"
#define DNS_STR_TYPE_NSEC3 "NSEC3"
#define DNS_STR_TYPE_NSEC3PARAM "NSEC3PARAM"
#define DNS_STR_TYPE_TLSA "TLSA"
#define DNS_STR_TYPE_SMIMEA "SMIMEA"
#define DNS_STR_TYPE_HIP "HIP"
#define DNS_STR_TYPE_NINFO "NINFO"
#define DNS_STR_TYPE_RKEY "RKEY"
#define DNS_STR_TYPE_TALINK "TALINK"
#define DNS_STR_TYPE_CDS "CDS"
#define DNS_STR_TYPE_CDNSKEY "CDNSKEY"
#define DNS_STR_TYPE_OPENPGPKEY "OPENPGPKEY"
#define DNS_STR_TYPE_CSYNC "CSYNC"
#define DNS_STR_TYPE_ZONEMD "ZONEMD"
#define DNS_STR_TYPE_SVCB "SVCB"
#define DNS_STR_TYPE_HTTPS "HTTPS"
#define DNS_STR_TYPE_SPF "SPF"
#define DNS_STR_TYPE_UINFO "UINFO"
#define DNS_STR_TYPE_UID "UID"
#define DNS_STR_TYPE_GID "GID"
#define DNS_STR_TYPE_UNSPEC "UNSPEC"
#define DNS_STR_TYPE_NID "NID"
#define DNS_STR_TYPE_L32 "L32"
#define DNS_STR_TYPE_L64 "L64"
#define DNS_STR_TYPE_LP "LP"
#define DNS_STR_TYPE_EUI48 "EUI48"
#define DNS_STR_TYPE_EUI64 "EUI64"
#define DNS_STR_TYPE_TKEY "TKEY"
#define DNS_STR_TYPE_TSIG "TSIG"
#define DNS_STR_TYPE_IXFR "IXFR"
#define DNS_STR_TYPE_AXFR "AXFR"
#define DNS_STR_TYPE_MAILB "MAILB"
#define DNS_STR_TYPE_MAILA "MAILA"

#define DNS_STR_TYPE_ANY "ANY"

#define DNS_STR_TYPE_URI "URI"
#define DNS_STR_TYPE_CAA "CAA"
#define DNS_STR_TYPE_AVC "AVC"
#define DNS_STR_TYPE_DOA "DOA"
#define DNS_STR_TYPE_AMTREPLAY "AMTREPLAY"
#define DNS_STR_TYPE_TA "TA"
#define DNS_STR_TYPE_DLV "DLV"

uint16_t dns_type_to_int(const char* type);
char* int_to_dns_type(uint16_t type);

#endif
