#include <stdio.h>
#include <stdint.h>

#define ARENAS_IMPLEMENTATION
#define ARENAS_ZERO_AFTER_RESET
#include "arenas.h"

int main()
{
    Arena a = {0};

    uint32_t *n = arena_alloc(&a, sizeof(uint32_t));
    *n = 12345;

    char *str = arena_strdup(&a, "Hello, World!");

    printf("%ld\n", *n);
    printf("%s\n", str);

    arena_reset(&a);
    printf("%ld\n", *n);
    printf("%s\n", str);

    arena_free(&a);
}
