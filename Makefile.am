INCLUDES= \
	$(GLIB_CFLAGS) \
	$(PULSE_CFLAGS)

bin_PROGRAMS=musualizer

musualizer_SOURCES =	\
        defines.h       \
        analyzer.h      \
        analyzer.cpp    \
        display.h       \
        display.cpp     \
	main.cpp

musualizer_LDADD = \
	$(GLIB_LIBS) \
	$(PULSE_LIBS)
