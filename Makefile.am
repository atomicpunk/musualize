INCLUDES= \
	$(GLU_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(PULSE_CFLAGS)

bin_PROGRAMS=musualizer

musualizer_SOURCES =	\
        defines.h       \
        analyzer.h      \
        analyzer.cpp    \
        display.h       \
        display.cpp     \
        model.h         \
        model.cpp       \
	main.cpp
	
musualizer_LDADD = \
	$(GLU_LIBS) \
	$(GLIB_LIBS) \
	$(PULSE_LIBS)
