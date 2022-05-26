#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#define SAFEALLOC(var, Type)                          \
    if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
        err("not enough memory");

void err(const char *fmt,...);
