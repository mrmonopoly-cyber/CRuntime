#include "CScheduler.h"
#include "CRuntime/common/HAL/thread.h"

#include <assert.h>

#include <CResult.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>


#include <CRuntime/common/common.h>

int idle_task(void* in)
{
  CS* self= (CS*) in;

  while (1)
  {
    Context_switch(self->active_ctx, &self->ctx);
  }

  TODO("panic");
  while(1);
}

static void _CS_run(CTP* ctp)
{
  CS self = {0};
  Context idle={0};
  const ContextAction action ={
    .entry = idle_task,
    .arg = &self,
  };

  self.task_pool = ctp;

  CRESULT_FULL_MATCH(Thread_allocate_memory(),
      res,
      {
        self.idle_task_stack =res.low_addr;
        CRESULT_ERR_MATCH(Context_init(&idle, INIT_STACK_VIEW(res.low_addr, res.size), action),
            err,{
              UNUSED(err);
              TODO("failed init context for idle stack, for now panic");
              while (1);
            }
        );
      },
      {
        UNUSED(res);
        TODO("manage error in idle task stack allocation, for now panic");
        while(1);
      }
  );



  while (1)
  {
    self.active_ctx = &idle;
    CRESULT_OK_MATCH(CTP_next(self.task_pool, &self.ctx),
        res,self.active_ctx = &res->ctx;);

    Context_switch(&self.ctx, self.active_ctx);
  }

  TODO("panic");
  while(1);
}

void CS_init(CTP* const restrict task_pool)
{
  if (task_pool) {
    _CS_run(task_pool);
  }

  TODO("panic");
  while(1);
}
