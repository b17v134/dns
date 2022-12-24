AC_INIT([dns],[0.0.1])
AC_PREREQ([2.69])

AC_CONFIG_MACRO_DIR([m4])
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_PROG_CC
AC_PROG_INSTALL

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PROG_AR

LT_INIT

AC_CONFIG_FILES([
    Makefile
    python/Makefile
    src/Makefile 
    tests/Makefile
])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
PKG_CHECK_MODULES([OPENSSL], [openssl])

AC_ARG_ENABLE(
    [python],
    AS_HELP_STRING([--enable-python], [Build python package [default=no]]),
    [enable_python=$enablevar],
    [enable_python=no]
)

if test "x$enable_python" != "xno" ; then
    AM_PATH_PYTHON([3.8],, [:])
    AX_PYTHON_DEVEL()
    AX_PYTHON_MODULE([pip], [true])
    AX_PYTHON_MODULE([wheel], [true])
fi

AM_CONDITIONAL([ENABLE_PYTHON], [test "x$enable_python" != "xno"])

AC_OUTPUT