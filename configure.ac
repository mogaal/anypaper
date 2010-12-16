#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(anypaper, 2.3, cdvdt at sourceforge.net)
AC_CONFIG_SRCDIR([anypaper/anypaper.c])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

AC_PROG_LIBTOOL

GTK_DOC_CHECK(1.11)
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

dnl ================================================================
dnl Start of pkg-config checks
dnl ================================================================

PKG_CHECK_MODULES(ANYPAPER, [
	gtk+-2.0 >= 2.10.0
	glib-2.0 >= 2.12.0
])

ANYPAPER_LIBS="${ANYPAPER_LIBS}"
AC_SUBST(ANYPAPER_LIBS)
AC_SUBST(ANYPAPER_CFLAGS)

dnl ================================================================
dnl GConf related settings
dnl ================================================================

AC_CONFIG_FILES([Makefile
                 anypaper/Makefile
		 pixmaps/Makefile
		 docs/Makefile
		 docs/reference/Makefile])
AC_OUTPUT
