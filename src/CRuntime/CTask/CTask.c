#include "CTask.h"

#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>

static int _task_trampoline(Context* ctx, Context* caller)
{
  if (ctx) {
    ctx->__action.entry(ctx->__action.arg, caller);
  }
  TODO("panic");
  while(1);
  return 0;
}


char idle_stack[16384]; //FIXME: need an allocator

static int idle_fun(void* input, void* env)
{
  UNUSED(input);
  while(1)
  {
    TODO("time delay for idle task");
    // Context_switch(NULL, env);
  }
  return 0;
}

CRReturn CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  CTaskDescription idle_task = {
    .stack = INIT_STATIC_STACK(idle_stack),
    .arg = NULL,
    .entry = idle_fun,
  };

  TRY(CTP_add_task(self, idle_task));

  return OK();
}

CRReturn CTP_add_task(CTP* const restrict self, const CTaskDescription task)
{
  CTask* task_ref = NULL;

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    task_ref = &self->task_pool[i];

    if (task_ref->caller == NULL)
    {
      task_ref = &self->task_pool[i];
      TRY(Context_init(&task_ref->ctx,
            task.stack,
            (TaskAction){(entry)_task_trampoline, &task_ref->ctx, task_ref->caller}));

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

    if (task_ref->caller != NULL)
    {
      self->task_pool[i].caller = caller;
      //INFO: task is not removed from the queue
      return CRESULT_T_OK(CTPPopRes, &self->task_pool[i]);
    }
  }
  
  return ERR_CTPPORES(CR_STATUS_ERR_QTASK_EMPTY,"empty task queue, nothing to do");
}
#undef ERR_CTPPORES
