========================================================================
    DYNAMIC LINK LIBRARY : SpreeTracker Project Overview
========================================================================

This is the SpreeTracker BZFS Plugin. It causes the server to track and report
sprees of kills by individual players, as well as streaks of kills in quick
succession.

HOW TO BUILD:
	- Download current version of BZFlag source
	- Place the SpreeTracker/ directory inside the plugins/ directory
	- Edit the configure script, and add plugins/SpreeTracker/Makefile to ac_config_files
	- Edit the Makefile.in and Makefile.ac in plugins/, and add SpreeTracker to the list of SUBDIRS
	- Run ./configure --enable-shared (add <--disable-client> to speed things up)
	- In the base directory, run make
	- The compiled .so file should be located in plugins/SpreeTracker/.libs/
	