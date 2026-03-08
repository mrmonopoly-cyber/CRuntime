#include "CRuntime.h"

#define OK() T_OK(CRuntimeResult, (STATUS_OK))
#define ERR(...) T_ERR(CRuntimeResult, ((CRuntimeError){__VA_ARGS__}))
#define RUNTIME_RETURN RETURN_TYPE(CRuntimeResult)
#define TRY_ERR(func)                                                                             \
  do{                                                                                             \
    CRuntimeResult __res = (func);                                                                \
    if(IS_ERR(__res))return __res;                                                                \
  }while(0)

#define UNUSED(X) (void) (X)

RUNTIME_RETURN _master_scheduler_init(CRuntime* const self)
{
  UNUSED(self);

  return OK();
}

RUNTIME_RETURN CRuntime_init(CRuntime* const self)
{
  TRY_ERR(_master_scheduler_init(self));
  
  return OK();
}
