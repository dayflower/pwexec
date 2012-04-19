#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(pwexec, 0.1, daydream.trippers@gmail.com)
AM_INIT_AUTOMAKE([foreign dist-bzip2 no-dist-gzip no-installinfo])
AC_CONFIG_SRCDIR([pwexec.c])
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_PROG_PKG_CONFIG
m4_pattern_allow(PKG_CONFIG_LIBDIR)
PKG_CHECK_MODULES(GLIB, glib-2.0)
PKG_CHECK_MODULES(GNOME_KEYRING, gnome-keyring-1)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([getpass strcasecmp])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
