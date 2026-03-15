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

#define ALIGNED_(X) __attribute__ ((aligned(X)))

#define UNUSED(X) (void) (X)
#define INIT_DEFAULT {0}


#ifndef CR_MAX_NUM_OF_CORES
#error "maximum num of cores in the system are not been specified, \
  to define it define CR_MAX_NUM_OF_CORES globally using -DCRMAX__NUM_OF_CORES=[num of cores]"
#endif // !CR_MAX_NUM_OF_CORES
