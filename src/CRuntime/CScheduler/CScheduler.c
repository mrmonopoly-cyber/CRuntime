#include "CScheduler.h"
#include "CRuntime/CTP/CSQ/CSQ.h"
#include "CRuntime/common/HAL/debug.h"
#include "CRuntime/common/common.h"

#include <assert.h>

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

CRRETURN CS_init(CS* const self, CSQ* const restrict task_pool)
{
  ContextAction action ={0};

  assert(self);

  memset(self, 0, sizeof(*self));

  self->task_pool = task_pool;

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

CRRETURN CS_run(CS* const restrict self)
{
  assert(self);

  while (1)
  {
    CTask* task = NULL;
    self->active_ctx = &self->idle_ctx;
    CRESULT_OK_MATCH(CSQ_pop_try(self->task_pool),
        res,{
          task = res;
          res->caller = &self->ctx;
          self->active_ctx = &res->ctx;
        }
    );
    Context_switch(&self->ctx, self->active_ctx);
    if (task && task->entry)
    {
      //FIXME: task should return to general scheduler, now it's stuck on the same executor
      CRESULT_ERR_MATCH(CSQ_push_try(self->task_pool, task),
          err,{
            UNUSED(err);
            TODO("manage failure in reinserting the task");
            }
      );
    }
  }

  return OK();
}
