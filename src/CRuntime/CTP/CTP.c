#include "CTP.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>

static int _task_trampoline(void* arg1)
{
  CTask* task = (CTask*)arg1;

  if (task)
  {
    task->entry(task->arg);
  }
  task->entry = NULL;
  Context_switch(&task->ctx, task->caller);
  TODO("panic task trampoline");
  while(1);
  return 0;
}

CRRETURN CTP_init(CTP* const restrict self, const size_t num_active_cores)
{
  assert(self);

  memset(self, 0, sizeof(*self));

  for (size_t i=0; i<num_active_cores; i++)
  {
    TRY(CSQ_init(&self->exec_queue[i]));
  }

  self->active_cores = num_active_cores;

  return OK();
}

CRRETURN CTP_add_task(CTP* const restrict self, const CTaskDescription task)
{
  assert(self);
  assert(task.entry);
  assert(task.stack.start_addr);
  assert(task.stack.size);

  size_t best =0;
  size_t size =CSQ_size(&self->exec_queue[0]);
  size_t new_size = 0;
  CTask* p_task = NULL;
  ContextAction action ={
    .entry = _task_trampoline,
    .arg = NULL,
  };

  while(!atomic_flag_test_and_set(&self->lock)); //FIXME: horrible

  //INFO: inefficient
  for(size_t i=0; i<self->active_cores; i++)
  {
    new_size =CSQ_size(&self->exec_queue[i]);
    if (!new_size)
    {
      best = i;
      break;
    }else if(new_size < size)
    {
      best = i;
    }
  }

  if (new_size >= CSQ_CAPACITY)
  {
    //HACK: this approach is incorrect since i should still be able to add the task
    //in the general queue, but for now until i figure a better way to store tasks it will
    //prevent errors
    return ERR(CR_STATUS_ERR_FULL, "all workers are full, task cannot be added");
  }


  //INFO: inefficient
  for(size_t i=0; i<CTP_CAPACITY; i++)
  {
    if (self->list[i].entry == NULL)
    {
      p_task = &self->list[i];
      p_task->entry = task.entry;
      p_task->arg = task.arg;

      action.arg = p_task;

      CRESULT_ERR_MATCH(Context_init(&p_task->ctx, task.stack, action),
          err,{
            memset(p_task, 0, sizeof(*p_task));
            return ERR(err.status, err.description);
          }
      );

      CRESULT_ERR_MATCH(CSQ_push_try(&self->exec_queue[best], p_task),
          err,{
            memset(p_task, 0, sizeof(*p_task));
            return ERR(err.status, err.description);
          }
      );
      break;
    }
  }

  atomic_flag_clear(&self->lock);

  return OK();
}
