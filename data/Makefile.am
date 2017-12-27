SUBDIRS = build_tools icons images templates

dist_pkgdata_DATA =	\
	completion.xml	\
	latexhelp.html

desktopdir = $(datadir)/applications
desktop_DATA = org.gnome.gnome-latex.desktop
@INTLTOOL_DESKTOP_RULE@

@INTLTOOL_XML_RULE@

appstream_XML = org.gnome.gnome-latex.appdata.xml
@APPSTREAM_XML_RULES@

gsettings_SCHEMAS = org.gnome.gnome-latex.gschema.xml
@GSETTINGS_RULES@

dbusservicedir = $(datadir)/dbus-1/services
dbusservice_DATA = org.gnome.gnome-latex.service

org.gnome.gnome-latex.service: Makefile
	$(AM_V_GEN) (echo '[D-BUS Service]'; \
		     echo 'Name=org.gnome.gnome-latex'; \
		     echo 'Exec=${bindir}/gnome-latex --gapplication-service') > $@.tmp && \
		     mv $@.tmp $@

EXTRA_DIST =					\
	org.gnome.gnome-latex.appdata.xml.in	\
	org.gnome.gnome-latex.desktop.in	\
	org.gnome.gnome-latex.gschema.xml.in

DISTCLEANFILES =				\
	org.gnome.gnome-latex.gschema.xml

CLEANFILES =					\
	org.gnome.gnome-latex.appdata.xml	\
	org.gnome.gnome-latex.desktop		\
	org.gnome.gnome-latex.service

-include $(top_srcdir)/git.mk
