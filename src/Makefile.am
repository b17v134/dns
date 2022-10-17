lib_LTLIBRARIES = libdns.la
libdns_la_SOURCES =                     \
    classes.c                           \
    dns_resolv.c                        \
    print.c                             \
    read.c                              \
    types.c                             \
    utils.c
libdns_la_CFLAGS=-I${top_srcdir}/include
libdns_la_LDFLAGS = -module -avoid-version -export-dynamic
otherincludedir = $(includedir)/dns
otherinclude_HEADERS =                  \
    ${top_srcdir}/include/classes.h     \
    ${top_srcdir}/include/dns_resolv.h  \
    ${top_srcdir}/include/dns.h         \
    ${top_srcdir}/include/print.h       \
    ${top_srcdir}/include/read.h        \
    ${top_srcdir}/include/types.h

bin_PROGRAMS = resolv
resolv_DEPENDENCIES=libdns.la
resolv_SOURCES = resolv.c
resolv_CFLAGS=-I${top_srcdir}/include
resolv_LDADD = -ldns