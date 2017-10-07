#include <stdio.h>
#include <stdint.h>

int power(int x, int y)
{
    int p = x;
    int r = 1;

    while (y) {
        if (y & 0x1) {
            r = r * p;
        }
        p = p * p;
        y = y >> 1;
    }
	
    return r;
}

int main()
{
    printf("%d\n", power(-501, 3));
}

