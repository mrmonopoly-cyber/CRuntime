#include "CRuntime.h"
#include <stddef.h>

#define UNUSED(X) (void) (X)

#define OK() T_OK(CRuntimeResult, (STATUS_OK))
#define ERR(...) T_ERR(CRuntimeResult, ((CRuntimeError){__VA_ARGS__}))
#define RUNTIME_RETURN RETURN_TYPE(CRuntimeResult)
#define TRY_ERR(func)                                                                             \
  do{                                                                                             \
    CRuntimeResult __res = (func);                                                                \
    if(IS_ERR(__res))return __res;                                                                \
  }while(0)


static RUNTIME_RETURN _master_scheduler_init(CRuntime* const self)
{
  UNUSED(self);

  return OK();
}

RUNTIME_RETURN _CRuntime_init(CRuntime* const self, const CRuntimeInitOpt opt)
{
  UNUSED(opt);
  TRY_ERR(_master_scheduler_init(self));
  
  return OK();
}

RUNTIME_RETURN CRuntime_add_task(CRuntime* const self, CTask task)
{
  UNUSED(self);
  UNUSED(task);
  size_t old_size, new_size =0;

  old_size = C_VECTOR_SIZE(self->task_waiting_queue);
  C_VECTOR_PUSH(self->task_waiting_queue, task);
  new_size= C_VECTOR_SIZE(self->task_waiting_queue);

  if (old_size == new_size) return ERR(
      .status = STATUS_FAILED_SAVING_NEW_TASK,
      .description = "failed to store the new given sruct in the internal data structure");

  return OK();
}
