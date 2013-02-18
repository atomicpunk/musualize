/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    double n, d = pow(2.0, 1.0/12.0);
    int i, j, k=0;
    for(i = 0, n = 27.5; i < 10; i++, n*=2)
        for(j = -9; j < 3; j++)
            printf ("%d %d %d %.12lf\n", k++, i, j+9, n*pow(d, j));
    return 0;
}
