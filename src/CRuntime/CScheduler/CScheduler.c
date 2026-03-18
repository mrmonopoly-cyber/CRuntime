#include "CScheduler.h"
#include "CRuntime/CTP/CSQ/CSQ.h"

#include <assert.h>
#include <stdatomic.h>

#define CHECK_CS_INPUT(cs) \
  if (!cs) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CS_init: cs ptr is null")

int idle_task(void* in)
{
  CS* self= (CS*) in;

  while (self)
  {
    Context_switch(self->active_ctx, &self->ctx);
  }

  TODO("idle_task: panic");
  while(1);
}

CRRETURN CS_init(CS* const self)
{
  ContextAction action ={0};

  assert(self);

  memset(self, 0, sizeof(*self));

  TRY(CSQ_init(&self->drain_queue));
  TRY(CSQ_init(&self->local_queue));
  for(size_t i=0; i<__NUM_TaskType; i++)
  {
    TRY(CSQ_init(&self->world_task_queue[i]));
  }

  action.arg = self;
  action.entry = idle_task;

  CRESULT_FULL_MATCH(Thread_allocate_memory(),
      res,
      {
        StackView idle_stack = INIT_STACK_VIEW(res.low_addr, res.size);
        self->idle_task_stack =res.low_addr;
        TRY(Context_init(&self->idle_ctx, idle_stack, action));
      },
      {
        UNUSED(res);
        TODO("manage error in idle task stack allocation, for now panic");
        while(1);
      }
  );

  return OK();
}

static inline 
void _CS_update_load(CS* const restrict self)
{
  size_t load=0;
  load =0;
  load += CSQ_size(&self->local_queue);
  load += CSQ_size(&self->drain_queue);
  for(TaskType t=TaskType_System; t<__NUM_TaskType; t++)
  {
    load += CSQ_size(&self->world_task_queue[t]);
  }
  atomic_store(&self->load, load);
}

static
void _CS_manage_task(CS* const restrict self, CTask* const task)
{
  task->caller = &self->ctx;
  self->active_ctx = &task->ctx;
  Context_switch(&self->ctx, self->active_ctx);

  //INFO: checking if task yield or terminated.
  if (task && task->entry)
  {
    CRESULT_OK_MATCH(CSQ_push_try(&self->local_queue, task),
        ok,{
          UNUSED(ok);
          return;
        }
    );

    CRESULT_ERR_MATCH(CSQ_push_try(&self->drain_queue, task),
        err,{
          UNUSED(err);
          TODO("unable to insert the task nor in the local_queue nor in drain_queue");
        }
    );
  }
}

CRRETURN CS_run(CS* const restrict self)
{
  assert(self);

  while (1)
  {
    _CS_update_load(self);

    CRESULT_OK_MATCH(CSQ_pop_try(&self->world_task_queue[TaskType_System]),
        res,{
          _CS_manage_task(self, res);
          continue;
        }
    );

    CRESULT_OK_MATCH(CSQ_pop_try(&self->local_queue),
        res,{
          _CS_manage_task(self, res);
          continue;
        }
    );

    for(TaskType t=TaskType_System + 1; t<__NUM_TaskType; t++)
    {
      CRESULT_OK_MATCH(CSQ_pop_try(&self->world_task_queue[t]),
          res,{
            _CS_manage_task(self, res);
            continue;
          }
      );
    }

    self->active_ctx = &self->idle_ctx;
    Context_switch(&self->ctx, self->active_ctx);
  }

  return OK();
}
