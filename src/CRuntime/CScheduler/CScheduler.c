#include "CScheduler.h"

#include <assert.h>

#include <CResult.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>


#include <CRuntime/common/common.h>

char stack_d[16384]__attribute__((__aligned__(16)));
int debug_task(void* in, void* env)
{
  CS* self= (CS*) env;
  UNUSED(in);

  while (1)
  {
    Context_switch(self->active_ctx, &self->ctx);
  }

  TODO("panic");
  while(1);
}

static void _CS_run(CTP* ctp)
{
  Context idle={0};
  CS self = {0};
  self.task_pool = ctp;
  const TaskAction action ={
    .entry = debug_task,
    .arg = NULL,
    .env = &self,
  };

  CRESULT_ERR_MATCH(Context_init(&idle, INIT_STATIC_STACK(stack_d), action),
      err,{
        UNUSED(err);
        TODO("failed init context for idle stack");
        while (1);
      });

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
