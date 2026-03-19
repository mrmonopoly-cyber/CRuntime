#pragma once

#include <stddef.h>


void *cr_memset(void *s, int c, size_t n);

size_t cr_strlen(const char* str);

int cr_vsnprintf(char *restrict s, size_t n, const char *restrict format, ...);

size_t cr_itoa(int d, char* s, size_t radix);

#define STRINGIFY(X) #X

#if defined(VERBOSE)
#define DO_PRAGMA(x) _Pragma (#x)
#define WARNING(msg) DO_PRAGMA(message (msg))
#else
#define WARNING(msg)
#endif // defined(VERBOSE)

#define ALIGNED_(X) __attribute__ ((aligned(X)))

#define UNUSED(X) (void) (X)
#define INIT_DEFAULT {0}

#ifndef CR_MAX_NUM_OF_CORES
#error "maximum num of cores in the system are not been specified, \
  to define it define CR_MAX_NUM_OF_CORES globally using -DCRMAX__NUM_OF_CORES=[num of cores]"
#endif // !CR_MAX_NUM_OF_CORES
