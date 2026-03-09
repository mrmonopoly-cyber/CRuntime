#pragma once

#include "errors/errors.h"

#define UNUSED(X) (void) (X)

#if  defined(__linux__) || defined(__gnu_linux__)

#include <fcntl.h>

typedef pid_t CRuntimePid;

#else

#error "platform not supported yet"

#endif // defined(__linux__) || defined(__gnu_linux__)

#define INIT_DEFAULT 0
