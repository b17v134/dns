TESTS = check_dns_resolv
check_PROGRAMS = check_dns_resolv
check_dns_resolv_SOURCES = check_dns_resolv.c
check_dns_resolv_CFLAGS = @CHECK_CFLAGS@ -I${top_builddir}/include
check_dns_resolv_LDADD = $(top_builddir)/src/libdns.la @CHECK_LIBS@