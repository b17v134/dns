TESTS = \
    check_types \
    check_write

check_PROGRAMS = \
    check_types \
    check_write

check_types_SOURCES = check_types.c
check_types_CFLAGS = @CHECK_CFLAGS@ -Wall -Werror -pedantic -I${top_srcdir}/include
check_types_LDADD = $(top_builddir)/src/libdns.la @CHECK_LIBS@

check_write_SOURCES = check_write.c
check_write_CFLAGS = @CHECK_CFLAGS@ -Wall -Werror -pedantic -I${top_srcdir}/include
check_write_LDADD = $(top_builddir)/src/libdns.la @CHECK_LIBS@
