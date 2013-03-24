/***
  This file is based on pactl from pulseaudio

  Copyright 2013 Todd Brandt <tebrandt@frontier.com>

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <pulse/pulseaudio.h>
#include "defines.h"
#include "analyzer.h"
#include "display.h"
#include "model.h"

static pa_context *context = NULL;
static pa_stream *stream = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static void *buffer = NULL;
static size_t buffer_length = 0, buffer_index = 0;
static char *stream_name = NULL, *client_name = NULL, *device = NULL;
int verbose = 0;

static pa_sample_spec sample_spec = {
    PA_SAMPLE_S16LE,
    44100,
    1
};

static pa_channel_map channel_map;
static int channel_map_set = 0;
static int flags = 0;

Analyzer *analyzer = NULL;

/* A shortcut for terminating the application */
static void quit(int ret) {
    assert(mainloop_api);
    mainloop_api->quit(mainloop_api, ret);
}

/* This is called whenever new data may is available */
static void stream_read_callback(pa_stream *s, size_t length, void *userdata) {
    const void *data;
    assert(s);
    assert(length > 0);

    if (pa_stream_peek(s, &data, &length) < 0) {
        fprintf(stderr, "pa_stream_peek() failed: %s\n", pa_strerror(pa_context_errno(context)));
        quit(1);
        return;
    }

    assert(data);
    assert(length > 0);

    analyzer->soundinput((unsigned char *)data, length);
    pa_stream_drop(s);
    if(analyzer->samples >= (sample_spec.rate * OUTPUT_DELAY_MSEC / 1000))
    {
#ifdef DISPLAYASCII
        analyzer->print();
#else
        analyzer->snapshot();
        Display::doRedraw();
#endif
    }
}

/* This is called whenever the context status changes */
static void context_state_callback(pa_context *c, void *userdata) {
    assert(c);

    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY: {
            int r;
            pa_buffer_attr buffer_attr;

            assert(c);
            assert(!stream);

            if (verbose)
                fprintf(stderr, "Connection established.\n");

            if (!(stream = pa_stream_new(c, stream_name, &sample_spec, channel_map_set ? &channel_map : NULL))) {
                fprintf(stderr, "pa_stream_new() failed: %s\n", pa_strerror(pa_context_errno(c)));
                goto fail;
            }

            pa_stream_set_read_callback(stream, stream_read_callback, NULL);

            if ((r = pa_stream_connect_record(stream, device, NULL, (pa_stream_flags_t)flags)) < 0) {
                fprintf(stderr, "pa_stream_connect_record() failed: %s\n", pa_strerror(pa_context_errno(c)));
                goto fail;
            }
            break;
        }

        case PA_CONTEXT_TERMINATED:
            quit(0);
            break;

        case PA_CONTEXT_FAILED:
        default:
            fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
            goto fail;
    }

    return;

fail:
    quit(1);

}

static void help(const char *argv0) {

    printf("%s [options]\n\n"
           "  -h, --help                            Show this help\n"
           "      --version                         Show version\n\n"
           "  -v, --verbose                         Enable verbose operations\n\n"
           "  -s, --server=SERVER                   The name of the server to connect to\n"
           "  -d, --device=DEVICE                   The name of the sink/source to connect to\n"
           "  -n, --client-name=NAME                How to call this client on the server\n"
           "      --stream-name=NAME                How to call this stream on the server\n"
           "      --rate=SAMPLERATE                 The sample rate in Hz (defaults to 44100)\n"
           "      --format=SAMPLEFORMAT             The sample type, one of s16le, s16be, u8, float32le,\n"
           "                                        float32be, ulaw, alaw, s32le, s32be (defaults to s16ne)\n"
           "      --channels=CHANNELS               The number of channels, 1 for mono, 2 for stereo\n"
           "                                        (defaults to 2)\n"
           "      --channel-map=CHANNELMAP          Channel map to use instead of the default\n"
           ,
           argv0);
}

void *pulseMainLoop(void *argument)
{
    int ret = 1;
    pa_mainloop* m = (pa_mainloop*)argument;
    if (pa_mainloop_run(m, &ret) < 0) {
        fprintf(stderr, "pa_mainloop_run() failed.\n");
    }
    return NULL;
}

enum {
    ARG_VERSION = 256,
    ARG_STREAM_NAME,
    ARG_SAMPLERATE,
    ARG_SAMPLEFORMAT,
    ARG_CHANNELS,
    ARG_CHANNELMAP
};

int main(int argc, char *argv[]) {
    pa_mainloop* m = NULL;
    pthread_t pulse_thread;
    int ret = 1, r, c;
    char *bn, *server = NULL, *window = NULL, *tonemap = NULL;

    glutInit(&argc, argv);

    static const struct option long_options[] = {
        {"device",       1, NULL, 'd'},
        {"server",       1, NULL, 's'},
        {"client-name",  1, NULL, 'n'},
        {"stream-name",  1, NULL, ARG_STREAM_NAME},
        {"version",      0, NULL, ARG_VERSION},
        {"help",         0, NULL, 'h'},
        {"verbose",      0, NULL, 'v'},
        {"rate",         1, NULL, ARG_SAMPLERATE},
        {"format",       1, NULL, ARG_SAMPLEFORMAT},
        {"channels",     1, NULL, ARG_CHANNELS},
        {"channel-map",  1, NULL, ARG_CHANNELMAP},
        {"window",       1, NULL, 'w'},
        {"tonemap",      1, NULL, 't'},
        {NULL,           0, NULL, 0}
    };

    if (!(bn = strrchr(argv[0], '/')))
        bn = argv[0];
    else
        bn++;

    while ((c = getopt_long(argc, argv, "d:s:n:hvw:t:", long_options, NULL)) != -1) {

        switch (c) {
            case 'h' :
                help(bn);
                ret = 0;
                goto quit;

            case ARG_VERSION:
                printf("pacat %s\nCompiled with libpulse %s\nLinked with libpulse %s\n", PACKAGE_VERSION, pa_get_headers_version(), pa_get_library_version());
                ret = 0;
                goto quit;

            case 'd':
                pa_xfree(device);
                device = pa_xstrdup(optarg);
                break;

            case 's':
                pa_xfree(server);
                server = pa_xstrdup(optarg);
                break;

            case 'n':
                pa_xfree(client_name);
                client_name = pa_xstrdup(optarg);
                break;

            case ARG_STREAM_NAME:
                pa_xfree(stream_name);
                stream_name = pa_xstrdup(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            case ARG_CHANNELS:
                sample_spec.channels = (uint8_t) atoi(optarg);
                break;

            case ARG_SAMPLEFORMAT:
                sample_spec.format = pa_parse_sample_format(optarg);
                break;

            case ARG_SAMPLERATE:
                sample_spec.rate = (uint32_t) atoi(optarg);
                break;

            case ARG_CHANNELMAP:
                if (!pa_channel_map_parse(&channel_map, optarg)) {
                    fprintf(stderr, "Invalid channel map '%s'\n", optarg);
                    goto quit;
                }

                channel_map_set = 1;
                break;
            case 'w':
                if(!strcmp(optarg, "hamming")||!strcmp(optarg, "blackman"))
                {
                    window = pa_xstrdup(optarg);
                }
                else
                {
                    fprintf(stderr, "%s is not a valid window algorithm\n", optarg);
                    goto quit;
                }
                break;

            case 't':
                if(Analyzer::tonemap(optarg))
                {
                    tonemap = pa_xstrdup(optarg);
                }
                else
                {
                    fprintf(stderr, "%s is not a valid tone mapping\n", optarg);
                    goto quit;
                }
                break;

            default:
                goto quit;
        }
    }

    if (!pa_sample_spec_valid(&sample_spec)) {
        fprintf(stderr, "Invalid sample specification\n");
        goto quit;
    }

    if (channel_map_set && pa_channel_map_compatible(&channel_map, &sample_spec)) {
        fprintf(stderr, "Channel map doesn't match sample specification\n");
        goto quit;
    }

    if (verbose) {
        char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
        pa_sample_spec_snprint(t, sizeof(t), &sample_spec);
        fprintf(stderr, "Opening a recording stream with specification '%s'.\n", t);
    }

    if (!(optind >= argc)) {
        if (optind+1 == argc) {
            int fd;

            if ((fd = open(argv[optind], O_WRONLY|O_TRUNC|O_CREAT, 0666)) < 0) {
                fprintf(stderr, "open(): %s\n", strerror(errno));
                goto quit;
            }

            if (dup2(fd, 1) < 0) {
                fprintf(stderr, "dup2(): %s\n", strerror(errno));
                goto quit;
            }

            close(fd);

            if (!stream_name)
                stream_name = pa_xstrdup(argv[optind]);

        } else {
            fprintf(stderr, "Too many arguments.\n");
            goto quit;
        }
    }

    analyzer = Analyzer::create(sample_spec.rate,
        pa_sample_size_of_format(sample_spec.format),
        sample_spec.channels, window, tonemap);
    #ifndef DISPLAYASCII
    Display::create();
    #endif

    if (!client_name)
        client_name = pa_xstrdup(bn);

    if (!stream_name)
        stream_name = pa_xstrdup(client_name);

    /* Set up a new main loop */
    if (!(m = pa_mainloop_new())) {
        fprintf(stderr, "pa_mainloop_new() failed.\n");
        goto quit;
    }

    mainloop_api = pa_mainloop_get_api(m);

    r = pa_signal_init(mainloop_api);
    assert(r == 0);
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
    /* Create a new connection context */
    if (!(context = pa_context_new(mainloop_api, client_name))) {
        fprintf(stderr, "pa_context_new() failed.\n");
        goto quit;
    }

    pa_context_set_state_callback(context, context_state_callback, NULL);

    /* Connect the context */
    if (pa_context_connect(context, server, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        fprintf(stderr, "pa_context_connect() failed: %s\n", pa_strerror(pa_context_errno(context)));
        goto quit;
    }

#ifdef DISPLAYASCII
    if (pa_mainloop_run(m, &ret) < 0) {
        fprintf(stderr, "pa_mainloop_run() failed.\n");
        goto quit;
    }
#else
    pthread_create(&pulse_thread, NULL, pulseMainLoop, m);
    glutMainLoop();
#endif
quit:
    if (stream)
        pa_stream_unref(stream);

    if (context)
        pa_context_unref(context);

    if (m) {
        pa_signal_done();
        pa_mainloop_free(m);
    }

    pa_xfree(buffer);
    pa_xfree(server);
    pa_xfree(device);
    pa_xfree(client_name);
    pa_xfree(stream_name);

    return ret;
}
