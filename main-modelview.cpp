/*
 * Copyright 2013 Todd Brandt <tebrandt@frontier.com>
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "modeldisplay.h"

int verbose = 0;

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
        fprintf(stderr, "USAGE: modelview <stlfile>\n");
        return -1;
    }

    glutInit(&argc, argv);
    createDisplay(argv[1]);
    glutMainLoop();
    return 0;
}
