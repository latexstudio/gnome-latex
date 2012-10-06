#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Init Autoconf
AC_PREREQ([2.64])

AC_INIT([LaTeXila],
        [2.6.1],
        [https://bugzilla.gnome.org/enter_bug.cgi?product=latexila],
	[latexila],
	[http://projects.gnome.org/latexila])

AC_CONFIG_SRCDIR([src/latexila.vala])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Init Automake
AM_INIT_AUTOMAKE([1.11 foreign tar-ustar no-dist-gzip dist-xz -Wno-portability])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

# Init Libtool
LT_PREREQ([2.2])
LT_INIT

# Required dependencies versions
GLIB_REQUIRED_VERSION="2.32"
GTK_REQUIRED_VERSION="3.4.3"
GTKSOURCEVIEW_REQUIRED_VERSION="3.4.1"
VALA_REQUIRED_VERSION="0.17.3.1"

AC_SUBST([GLIB_REQUIRED_VERSION])
AC_SUBST([GTK_REQUIRED_VERSION])
AC_SUBST([GTKSOURCEVIEW_REQUIRED_VERSION])
AC_SUBST([VALA_REQUIRED_VERSION])

# Some directories

# adl_RECURSIVE_EVAL(VALUE, RESULT)
# =================================
# Interpolate the VALUE in loop until it doesn't change,
# and set the result to $RESULT.
# WARNING: It's easy to get an infinite loop with some unsane input.
# For example ${datadir} becomes ${datarootdir}, and then ${prefix}/share, and
# finally ${prefix} is replaced by the prefix.
AC_DEFUN([adl_RECURSIVE_EVAL],
[_lcl_receval="$1"
$2=`(test "x$prefix" = xNONE && prefix="$ac_default_prefix"
     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
     _lcl_receval_old=''
     while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
       _lcl_receval_old="[$]_lcl_receval"
       eval _lcl_receval="\"[$]_lcl_receval\""
     done
     echo "[$]_lcl_receval")`])

adl_RECURSIVE_EVAL(["${datadir}/latexila"], [DATA_DIR])
AC_DEFINE_UNQUOTED([DATA_DIR],
                   ["${DATA_DIR}"],
		   [latexila data directory])

adl_RECURSIVE_EVAL(["${datadir}/icons/hicolor"], [ICONS_DIR])
AC_DEFINE_UNQUOTED([ICONS_DIR],
                   ["${ICONS_DIR}"],
		   [icons directory])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_VALAC([${VALA_REQUIRED_VERSION}])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES], [glib-compile-resources])

# Checks for dependencies
PKG_CHECK_MODULES([LATEXILA], [
	glib-2.0 >= ${GLIB_REQUIRED_VERSION}
	gio-2.0 >= ${GLIB_REQUIRED_VERSION}
	gtk+-3.0 >= ${GTK_REQUIRED_VERSION}
	gtksourceview-3.0 >= ${GTKSOURCEVIEW_REQUIRED_VERSION}
	gsettings-desktop-schemas
	gee-1.0
	enchant
])

AC_SUBST([LATEXILA_CFLAGS])
AC_SUBST([LATEXILA_LIBS])

# Native Language Support
IT_PROG_INTLTOOL([0.40.0])
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=${PACKAGE_TARNAME}
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
                   ["${GETTEXT_PACKAGE}"],
                   [The domain to use with gettext])

adl_RECURSIVE_EVAL(["${localedir}"], [LOCALE_DIR])
AC_DEFINE_UNQUOTED([LOCALE_DIR],
                   ["${LOCALE_DIR}"],
		   [locale directory])

# Help documentation
YELP_HELP_INIT

# GSettings
GLIB_GSETTINGS

adl_RECURSIVE_EVAL(["${datadir}"], [SCHEMA_DIR])
AC_DEFINE_UNQUOTED([SCHEMA_DIR],
                   ["${SCHEMA_DIR}"],
		   [GSettings schemas directory])

# Generate files
AC_CONFIG_FILES([Makefile
		 data/Makefile
		 data/build_tools/Makefile
		 data/latexila.desktop.in
		 data/icons/Makefile
		 data/icons/16x16/Makefile
		 data/icons/22x22/Makefile
		 data/icons/24x24/Makefile
		 data/icons/32x32/Makefile
		 data/icons/48x48/Makefile
		 data/icons/192x192/Makefile
		 data/images/Makefile
		 data/images/app/Makefile
		 data/org.gnome.latexila.gschema.xml.in
		 data/templates/Makefile
		 help/Makefile
		 man/Makefile
		 po/Makefile.in
                 src/Makefile
		 src/gedit/Makefile
		 src/gtkspell/Makefile
		 src/ui/Makefile
                 README])

AC_OUTPUT

# Print a summary of the configuration
echo "

Configuration:

    ${PACKAGE_NAME} version ${PACKAGE_VERSION}

    Prefix: ${prefix}
    Compiler: ${CC} ${CFLAGS} ${CPPFLAGS}
"