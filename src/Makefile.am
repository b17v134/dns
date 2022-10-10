lib_LTLIBRARIES = libdns.la
libdns_la_SOURCES = dns.c
libdns_la_CFLAGS=-I${top_srcdir}/include
libdns_la_LDFLAGS = -module -avoid-version -export-dynamic

bin_PROGRAMS = resolv
resolv_SOURCES = main.c
resolv_CFLAGS=-I${top_srcdir}/include
resolv_LDADD = -ldns