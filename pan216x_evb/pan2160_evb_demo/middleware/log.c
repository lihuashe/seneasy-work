#include "log.h"

void panlog_hexdump(size_t width, const unsigned char *buf, size_t size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf("%02X ", buf[i]);
        if (i % width == (width - 1))
            printf("\r\n");
    }
}

void printHex(unsigned char *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", buf[i]);
    }
    printf("\r\n");
}
