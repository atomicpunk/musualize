INCLUDES= \
	$(GLIB_CFLAGS) \
	$(PULSE_CFLAGS)

bin_PROGRAMS=musualizer

musualizer_SOURCES =	\
        analyzer.h      \
        analyzer.cpp      \
        goertzel.h      \
        goertzel.cpp    \
	main.cpp

musualizer_LDADD = \
	$(GLIB_LIBS) \
	$(PULSE_LIBS)
