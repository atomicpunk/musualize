SUBDIRS=bullet

INCLUDES= \
	$(GLU_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(BULLET_CFLAGS) \
	$(PULSE_CFLAGS)

bin_PROGRAMS=musualizer modelview bullettest

musualizer_SOURCES =	\
        defines.h       \
        analyzer.h      \
        analyzer.cpp    \
        display.h       \
        display.cpp     \
        musicdisplay.h  \
        musicdisplay.cpp \
	main-musualizer.cpp
	
musualizer_LDADD = \
	$(GLU_LIBS) \
	$(GLIB_LIBS) \
	$(PULSE_LIBS) -lpthread

modelview_SOURCES =	\
        defines.h       \
        display.h       \
        display.cpp     \
        modeldisplay.h  \
        modeldisplay.cpp \
        model.h         \
        model.cpp       \
	main-modelview.cpp

modelview_LDADD = \
	$(GLU_LIBS) \
	$(GLIB_LIBS)

bullettest_CXXFLAGS=-I@top_builddir@/bullet $(CXXFLAGS)

bullettest_SOURCES =	\
        bulletboxes.h      \
        bulletboxes.cpp    \
	main-bullettest.cpp

bullettest_LDADD = \
        -L@top_builddir@/bullet -lbulletopenglsupport \
	$(GLU_LIBS) \
	$(BULLET_LIBS) \
	$(GLIB_LIBS) \
	$(PULSE_LIBS)
