#include "CTask.h"

#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>

static int _task_trampoline(void* arg1, void* arg2)
{
  Context* ctx = (Context*)arg1;
  TODO("task_trapoline");

  if (ctx) {
    ((entry)arg2)(ctx->__action.arg, ctx->__action.env);
  }
  TODO("panic");
  while(1);
  return 0;
}


CRReturn CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return OK();
}

CRReturn CTP_add_task(CTP* const restrict self, const CTaskDescription task)
{
  CTask* task_ref = NULL;

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    task_ref = &self->task_pool[i];

    if (task_ref->ctx.__action.entry == NULL)
    {
      TRY(Context_init(&task_ref->ctx,
            task.stack,
            (TaskAction){_task_trampoline, &task_ref->ctx, (void*) (uintptr_t) task.entry}));

      return OK();
    }
  }

  return ERR(CR_STATUS_ERR_QTASK_FULL,"full task queue, unable to save the task");
}

#define ERR_CTPPORES(...)CRESULT_T_ERR(CTPPopRes, ((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict self, Context* const restrict caller)
{
  CTask* task_ref = NULL;

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    task_ref = &self->task_pool[i];

    if (task_ref->ctx.__action.entry != NULL &&  task_ref->caller == NULL)
    {
      self->task_pool[i].caller = caller;
      //INFO: task is not removed from the queue
      return CRESULT_T_OK(CTPPopRes, &self->task_pool[i]);
    }
  }
  
  return ERR_CTPPORES(CR_STATUS_ERR_QTASK_EMPTY,"empty task queue, nothing to do");
}
#undef ERR_CTPPORES
