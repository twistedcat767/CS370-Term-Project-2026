#undef NDEBUG
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    assert(CHAR_BIT == 8);
    assert(sizeof(int) >= 4);
    assert(sizeof(uint32_t) == 4);

    const char src[] = "sensor";
    char dst[sizeof src];
    memcpy(dst, src, sizeof src);
    assert(memcmp(dst, src, sizeof src) == 0);

    printf("test: all asserts passed\n");
    return 0;
}
