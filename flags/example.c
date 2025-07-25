#include <stdbool.h>
#include <stdio.h>

#define DEBUG
#define FLAGS_IMPLEMENTATION
#include "flags.h"

int main(int argc, char **argv)
{
    printf("Hello, World!\n");

    FlagContext flagctx = {0};

    bool  *verbose = flag_register_bool  (&flagctx, "verbose", "Displays extra info to standard output");
    int   *size    = flag_register_int   (&flagctx, "size", "Size of somethin idk");
    float *rate    = flag_register_float (&flagctx, "rate", "Rate of the something");
    char **path    = flag_register_string(&flagctx, "path", "Path of the something file");

    flag_parse(&flagctx, argc, argv);

    printf("Verbose: %s\n", *verbose ? "true" : "false");
    printf("Size: %ld\n", *size);
    printf("Rate: %f\n", *rate);
    printf("Path: %s\n", *path);

    flag_free(&flagctx);
}
