#pragma once

#include "errors/errors.h"
#include "HAL/HAL.h"

#define STRINGIFY(X) #X

#if defined(VERBOSE)
#define DO_PRAGMA(x) _Pragma (#x)
#define WARNING(msg) DO_PRAGMA(message (msg))
#else
#define WARNING(msg)
#endif // defined(VERBOSE)


#define UNUSED(X) (void) (X)
#define INIT_DEFAULT {0}
