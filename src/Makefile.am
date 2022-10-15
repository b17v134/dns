lib_LTLIBRARIES = libdns.la
libdns_la_SOURCES = dns.c dns_resolv.c types.c
libdns_la_CFLAGS=-I${top_srcdir}/include
libdns_la_LDFLAGS = -module -avoid-version -export-dynamic

bin_PROGRAMS = resolv
resolv_DEPENDENCIES=libdns.la
resolv_SOURCES = resolv.c
resolv_CFLAGS=-I${top_srcdir}/include
resolv_LDADD = -ldns