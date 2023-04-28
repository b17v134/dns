CFLAGS_JSON_C = `pkg-config --cflags json-c`
LDFLAGS_JSON_C = `pkg-config --libs json-c`

CFLAGS_NGHTTP2 = `pkg-config --cflags libnghttp2`
LDFLAGS_NGHTTP2 = `pkg-config --libs libnghttp2`

CFLAGS_LIBEVENT = `pkg-config --cflags libevent`
LDFLAGS_LIBEVENT = `pkg-config --libs libevent`

CFLAGS_LIBEVENT_OPENSSL = `pkg-config --cflags libevent_openssl`
LDFLAGS_LIBEVENT_OPENSSL = `pkg-config --libs libevent_openssl`

LINKER_FLAGS=`pkg-config --libs openssl` 
AM_CFLAGS = -Wall -Werror -pedantic -I$(top_srcdir)/include $(CFLAGS_JSON_C) $(CFLAGS_NGHTTP2) $(CFLAGS_LIBEVENT) $(CFLAGS_LIBEVENT_OPENSSL) 

lib_LTLIBRARIES = libdns.la
libdns_la_SOURCES =                     \
    classes.c                           \
    https.c                             \
    message.c                           \
    print.c                             \
    read.c                              \
    tls.c                               \
    types.c                             \
    udp.c                               \
    utils.c                             \
    write.c
libdns_la_LDFLAGS = -module -avoid-version -export-dynamic $(LINKER_FLAGS) $(LDFLAGS_JSON_C) $(LDFLAGS_NGHTTP2) $(LDFLAGS_LIBEVENT) $(LDFLAGS_LIBEVENT_OPENSSL)

otherincludedir = $(includedir)/dns
otherinclude_HEADERS =                    \
    $(top_builddir)/include/classes.h     \
    $(top_builddir)/include/dns.h         \
    $(top_builddir)/include/errors.h      \
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