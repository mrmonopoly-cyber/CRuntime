#include "CTask.h"

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>

typedef struct{
  CTask task_pool[TASK_POOL_MAX_CAPACITY];
  size_t cursor_index;
}__CTP;

static int _task_trampoline(void* arg1)
{
  CTask* task = (CTask*)arg1;

  if (task) {
    task->entry(task->ctx.__action.arg, task);
  }
  task->entry=NULL;
  Context_switch(&task->ctx, task->caller);
  TODO("panic task trampoline");
  while(1);
  return 0;
}


CRRETURN CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return OK();
}

//FIXME: this function is not thread safe
CRRETURN CTP_add_task(CTP* const restrict cs, const CTaskDescription task)
{
  __CTP* self = (__CTP*) cs;
  CTask* task_ref = NULL;
  const size_t task_pool_size = sizeof(self->task_pool)/sizeof(self->task_pool[0]);

  //TODO: more efficient
  for(uint16_t i=0; i<task_pool_size; i++)
  {
    task_ref = &self->task_pool[i];

    if (task_ref->entry == NULL)
    {
      task_ref->entry = task.entry;
      TRY(Context_init(&task_ref->ctx, task.stack,
            INIT_CONTEXT_ACTION(_task_trampoline, task_ref)));

      return OK();
    }
  }

  return ERR(CR_STATUS_ERR_QTASK_FULL,"full task queue, unable to save the task");
}

//FIXME: this function is not thread safe
#define ERR_CTPPORES(...)CRESULT_T_ERR(CTPPopRes, ((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict cs, Context* const restrict caller)
{
  __CTP* self = (__CTP*) cs;
  CTask* task_ref = NULL;
  const size_t task_pool_size = sizeof(self->task_pool)/sizeof(self->task_pool[0]);

  //TODO: more efficient
  for(uint16_t i=0;i<task_pool_size; i++)
  {
    self->cursor_index= (self->cursor_index+1)%task_pool_size;
    task_ref = &self->task_pool[self->cursor_index];

    //INFO: if a task has returned than cleanup the data to prevent issues
    if(task_ref->entry==NULL && task_ref->ctx.__action.entry)
    {
      memset(task_ref, 0, sizeof(*task_ref));
    }

    if (task_ref->ctx.__action.entry != NULL &&
        (task_ref->caller == NULL || task_ref->caller == caller ))
    {
      task_ref->caller =caller;
      //INFO: task is not removed from the queue
      return CRESULT_T_OK(CTPPopRes, task_ref);
    }
  }
  
  return ERR_CTPPORES(CR_STATUS_ERR_QTASK_EMPTY,"empty task queue, nothing to do");
}
#undef ERR_CTPPORES
