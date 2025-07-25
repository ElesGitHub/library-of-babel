#ifndef ARENAS_H_
#define ARENAS_H_

typedef struct Region Region;
struct Region {
    Region *next;

    size_t  capacity;
    size_t  used;
    char    data[];
};

typedef struct {
    Region *start, *end;
} Arena;

Region *arena_new_region(Arena *a, size_t size);
void *arena_alloc(Arena *a, size_t size);

void *arena_memcpy(Arena *a, const void *ptr, size_t size);
char *arena_strdup(Arena *a, const char *str);

void arena_reset(Arena *a);

void arena_free(Arena *a);

#endif // ARENAS_H_

/////////////////////////////////////////////////////////////////////

#ifdef ARENAS_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

#ifndef ARENAS_MIN_REGION_SIZE
#define ARENAS_MIN_REGION_SIZE 128
#endif // ARENAS_MIN_REGION_SIZE

Region *arena_new_region(Arena *a, size_t size)
{
    size_t rcap = size > ARENAS_MIN_REGION_SIZE ? size : ARENAS_MIN_REGION_SIZE;
    Region *region = malloc(sizeof(Region) + rcap); 

    region->capacity = rcap;
    region->used = 0;
    region->next = NULL;

    if (a->start == NULL) {
        a->start = region;
        a->end   = region;
    } else {
        a->end->next = region;
        a->end = region;
    }

#ifdef ARENAS_ZERO_AFTER_RESET
        bzero(region->data, region->capacity);
#endif

    return region;
}

void *arena_alloc(Arena *a, size_t size)
{
    Region *region = a->start;
    void *ptr = NULL;

    while (region != NULL) {
        if (region->capacity - region->used >= size) {
            ptr = &region->data[region->used];
            region->used += size;
            break;
        }
        region = region->next;
    }

    if (ptr == NULL) {
        region = arena_new_region(a, size);
        ptr = region->data;
        region->used += size;
    }
    return ptr;
}

void *arena_memcpy(Arena *a, const void *ptr, size_t size)
{
    return memcpy(
        arena_alloc(a, size), // dest
        ptr,                  // src
        size                  // n
    );
}

char *arena_strdup(Arena *a, const char *str)
{
    return (char *) arena_memcpy(a, str, (size_t) strlen(str) + 1);
}

void arena_reset(Arena *a)
{
    Region *region = a->start;
    while (region != NULL) {
#ifdef ARENAS_ZERO_AFTER_RESET
        bzero(region->data, region->capacity);
#endif
        region->used = 0;
        region = region->next;
    }
}

void arena_free(Arena *a)
{
    Region *region = a->start, *next;
    while (region != NULL) {
        next = region->next;
        free(region);
        region = next;
    }

    a->start = NULL;
    a->end   = NULL;
}

#endif // ARENAS_IMPLEMENTATION
