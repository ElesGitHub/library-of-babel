#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>

// c type, name, NAME
#define __FLAG_TYPES_MAP \
    X(bool,   bool,   BOOL) \
    X(int,    int,    INT)  \
    X(float,  float,  FLOAT) \
    X(char *, string, STRING)

typedef enum {
#define X(type, name, NAME) FLAG_##NAME,
    __FLAG_TYPES_MAP
#undef X
} FlagType;

// TODO Add alias support
typedef struct {
    char       *name;
    char       *desc;
    FlagType    type;
    char        val[8];
} Flag;

typedef struct {
    Flag         *definitions;    // TODO Evaluate difference in average lookup time and
    unsigned int  registered;     //      memory usage if using hashmap instead
    unsigned int  registered_cap;

//  Arena *a; // TODO Include proper arena allocator implementation,
              //      using malloc/free meanwhile
} FlagContext;

#define X(type, _name, _NAME)\
    static inline type *flag_register_##_name(FlagContext *ctx, char *name, char *desc);
    
    __FLAG_TYPES_MAP
#undef X

void flag_parse(FlagContext *ctx, int argc, char **argv);

void flag_free(FlagContext *ctx);

#ifdef FLAGS_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

#ifndef FLAGS_MIN_CAP
#define FLAGS_MIN_CAP 20
#endif // FLAGS_MIN_CAP

static void *flag_register(FlagContext *ctx, char *name, char *desc, FlagType type)
{
    if (ctx->registered >= ctx->registered_cap) {
        ctx->registered_cap = ctx->registered_cap < FLAGS_MIN_CAP 
                           ? FLAGS_MIN_CAP 
                           : ctx->registered_cap * 2;

        ctx->definitions = realloc(ctx->definitions, ctx->registered_cap * sizeof(Flag));
    }

    ctx->definitions[ctx->registered] = (Flag) {
        .name = strdup(name),
        .desc = strdup(desc),
        .type = type,
    };

    void *res = ctx->definitions[ctx->registered].val;
    ctx->registered++;

    return res;
}

#define X(type, _name, _NAME) \
    static inline type *flag_register_##_name(FlagContext *ctx, char *name, char *desc) {\
        return ( type *) flag_register(ctx, name, desc, FLAG_##_NAME);\
    }
    __FLAG_TYPES_MAP
#undef X

// Currently only checks arguments that start with '--'
void flag_parse(FlagContext *ctx, int argc, char **argv)
{
    Flag *flag;

    // Iterate all program arguments
    for (int i = 0; i < argc; ++i) {        
        char *arg = argv[i];
        if (strncmp(arg, "--", 2) == 0) {
            arg += 2;
            flag = NULL;

            // Find corresponding registered flag
            for (int j = 0; j < ctx->registered; ++j) {
                if (strcmp(arg, ctx->definitions[j].name) == 0) {
                    flag = ctx->definitions + j;
                    break;
                }
            }
            if (!flag) continue;

            // If necessary, get flag arguments
            // TODO Support flags with more than 1 argument (?)
            if (flag->type == FLAG_BOOL) {
                *flag->val = true;
            } else {
                if (i >= argc - 1) {
                    fprintf(stderr, "[ERROR] No arguments provided for flag '%s'\n", flag);
                    exit(1);
                }
                ++i;
                switch (flag->type) {
                case FLAG_INT:
                    *((int *) flag->val) = atoi(argv[i]);
                    break;
                case FLAG_FLOAT:
                    *((float *)flag->val) = atof(argv[i]);
                    break;
                case FLAG_STRING:
                    *((char **)flag->val) = argv[i];
                case FLAG_BOOL:
                default:
                    break;
                }
            }
        }
    }
}

void flag_free(FlagContext *ctx)
{
    // Free each registered flag
    for (int i = 0; i < ctx->registered; ++i) {
        Flag flag = ctx->definitions[i];
        free(flag.name);
        free(flag.desc);
    }

    free(ctx->definitions);
}

#endif // FLAGS_IMPLEMENTATION
#endif // FLAGS_H
