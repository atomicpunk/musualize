#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    double n, d = pow(2.0, 1.0/12.0);
    int i, j;
    for(i = 0, n = 27.5; i < 10; i++, n*=2)
        for(j = -9; j < 3; j++)
            printf ("%d %d %.12lf\n", i, j+9, n*pow(d, j));
    return 0;
}
