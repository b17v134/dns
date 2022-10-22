AM_CFLAGS = -Wall -Werror -I$(top_srcdir)/include

lib_LTLIBRARIES = libdns.la
libdns_la_SOURCES =                     \
    classes.c                           \
    dns_resolv.c                        \
    print.c                             \
    read.c                              \
    types.c                             \
    utils.c                             \
    write.c

libdns_la_LDFLAGS = -module -avoid-version -export-dynamic
otherincludedir = $(includedir)/dns
otherinclude_HEADERS =                    \
    $(top_builddir)/include/classes.h     \
    $(top_builddir)/include/dns_resolv.h  \
    $(top_builddir)/include/dns.h         \
    $(top_builddir)/include/message.h     \
    $(top_builddir)/include/print.h       \
    $(top_builddir)/include/read.h        \
    $(top_builddir)/include/types.h       \
    $(top_builddir)/include/utils.h       \
    $(top_builddir)/include/write.h

bin_PROGRAMS = resolv
resolv_DEPENDENCIES=libdns.la
resolv_SOURCES = resolv.c
resolv_LDADD = -ldns