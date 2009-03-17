## Makefile.am for kpegg

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kpegg kpegg_client

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# the library search path. 
kpegg_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
kpegg_LDADD   = $(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for kpegg
kpegg_SOURCES = main.cpp kpegg.cpp kpeggview.cpp \
		      pref.cpp kpeggiface.skel

# these are the headers for your project
noinst_HEADERS = kpegg.h kpeggview.h pref.h

# client stuff
kpegg_client_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kpegg_client_LDADD   = $(LIB_KDECORE)
kpegg_client_SOURCES = kpegg_client.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpegg.pot

KDE_ICON = kpegg

# this is where the kdelnk file will go 
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = kpegg.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/kpegg
rc_DATA = kpeggui.rc
