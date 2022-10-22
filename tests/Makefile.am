TESTS = check_types
check_PROGRAMS = check_types
check_types_SOURCES = check_types.c check_write.c
check_types_CFLAGS = @CHECK_CFLAGS@ -Wall -Werror -I${top_srcdir}/include
check_types_LDADD = $(top_builddir)/src/libdns.la @CHECK_LIBS@