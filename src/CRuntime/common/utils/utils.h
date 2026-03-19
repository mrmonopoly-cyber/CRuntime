#pragma once

#include <stddef.h>


void *cr_memset(void *s, int c, size_t n);

size_t cr_strlen(const char* str);

int cr_vsnprintf(char *restrict s, size_t n, const char *restrict format, ...);

size_t cr_itoa(int d, char* s, size_t radix);
