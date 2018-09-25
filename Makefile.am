pkglib_LTLIBRARIES = SpreeTracker.la

SpreeTracker_la_SOURCES = SpreeTracker.cpp
SpreeTracker_la_CXXFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
SpreeTracker_la_LDFLAGS = -module -avoid-version
SpreeTracker_la_LIBADD = $(top_srcdir)/plugins/plugin_utils/libplugin_utils.la


EXTRA_DIST = \
	README.txt \
	SpreeTracker.def \
	SpreeTracker.sln \
	SpreeTracker.vcproj

MAINTAINERCLEANFILES =	\
	Makefile.in
