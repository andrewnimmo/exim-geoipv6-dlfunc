AC_INIT([MaxMind GeoIP2 dlfunc for Exim], [0.1], [],
  [exim-geoipv6-dlfunc], [https://github.com/andrewnimmo/exim-geoipv6-dlfunc])

AC_CONFIG_SRCDIR([exim-geoipv6-dlfunc.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_PROG_CC
AC_PROG_CC_C99

AC_CHECK_HEADER([local_scan.h], [],
  [AC_MSG_ERROR([Required Exim local_scan.h include file was not found])])

AC_CHECK_HEADER([maxminddb.h], [],
  [AC_MSG_ERROR([MaxMind libmaxminddb C API maxminddb.h include file was not found])])
AC_CHECK_LIB([maxminddb], [MMDB_lookup_string], [],
  [AC_MSG_ERROR([MaxMind libmaxminddb C API library libmaxminddb not found])])

AC_OUTPUT
