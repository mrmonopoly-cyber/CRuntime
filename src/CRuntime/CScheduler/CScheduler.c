#include "CScheduler.h"
#include "CRuntime/common/errors/errors.h"

#include <assert.h>

#include <CResult.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>

#define CHECK_CS_INPUT(cs) \
  if (!cs) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CS_init: cs ptr is null")

typedef struct{
  CTP* task_pool;
  void* idle_task_stack;
  Context idle_ctx;
  Context ctx;
  Context* active_ctx;
}__CS;

int idle_task(void* in)
{
  __CS* self= (__CS*) in;

  while (self)
  {
    Context_switch(self->active_ctx, &self->ctx);
  }

  TODO("idle_task: panic");
  while(1);
}

CRRETURN CS_init(CS* const cs, CTP* const restrict task_pool)
{
  __CS* self = NULL;
  ContextAction action ={0};

  CHECK_CS_INPUT(cs);
  if (!task_pool) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CS_init: task_pool ptr is null");

  memset(cs, 0, sizeof(*cs));

  self = (__CS*) cs;
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

CRRETURN CS_run(CS* const restrict cs)
{
  __CS* self = NULL;

  CHECK_CS_INPUT(cs);

  self = (__CS*) cs;

  while (1)
  {
    self->active_ctx = &self->idle_ctx;
    CRESULT_OK_MATCH(CTP_next(self->task_pool, &self->ctx),
        res,self->active_ctx = &res->ctx;);

    Context_switch(&self->ctx, self->active_ctx);
  }

  return OK();
}
