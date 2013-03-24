INCLUDES= \
	$(GLU_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(PULSE_CFLAGS)

bin_PROGRAMS=musualizer modelview

musualizer_SOURCES =	\
        defines.h       \
        analyzer.h      \
        analyzer.cpp    \
        display.h       \
        display.cpp     \
        musicdisplay.h  \
        musicdisplay.cpp \
        model.h         \
        model.cpp       \
	main.cpp
	
musualizer_LDADD = \
	$(GLU_LIBS) \
	$(GLIB_LIBS) \
	$(PULSE_LIBS)

modelview_SOURCES =	\
        defines.h       \
        display.h       \
        display.cpp     \
        modeldisplay.h  \
        modeldisplay.cpp \
        model.h         \
        model.cpp       \
	modelview.cpp

modelview_LDADD = \
	$(GLU_LIBS) \
	$(GLIB_LIBS)
