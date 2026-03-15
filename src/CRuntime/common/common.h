#pragma once

#include <stdatomic.h>

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


#define CRQUEUE_TEMPLATE(TYPE, MAX_SIZE)\
  struct{\
    TYPE list[MAX_SIZE];\
    size_t stack[MAX_SIZE];\
    atomic_size_t cursor;\
  }


#define CRQUEUE_INIT(self) \
  do{\
    const size_t queue_size =sizeof((self)->list)/sizeof((self)->list[0]);\
    for(size_t i=0; i<queue_size; i++)\
    {\
      (self)->stack[i]=i;\
    }\
    atomic_init(&(self)->cursor, queue_size);\
  }while(0)

#define CRQUEUE_SIZE(self) (sizeof((self)->list)/sizeof((self)->list[0]))

#ifndef CR_MAX_NUM_OF_CORES
#error "maximum num of cores in the system are not been specified, \
  to define it define CR_MAX_NUM_OF_CORES globally using -DCRMAX__NUM_OF_CORES=[num of cores]"
#endif // !CR_MAX_NUM_OF_CORES
