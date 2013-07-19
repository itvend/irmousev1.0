#include "other.h"

const char *byte_to_binary(int x)
{
    static char b[15];
    b[0] = '\0';

    int z;
    for (z = 16384; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
const char *byte_to_binary2(int x)
{
    static char b[15];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}