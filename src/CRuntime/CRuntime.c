#include "CRuntime.h"

#include <assert.h>
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/CScheduler/CScheduler.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>

#define FOR_EACH_ENGINE_INDEX(i) for(size_t i=0; i<sizeof(self->engines)/sizeof(self->engines[0]); i++)

int _CS_trampoline(void* in)
{
  CTP* ctp = (CTP*)in;

  if (ctp) {
    CS_init(ctp);
  }

  TODO("panic");
  while(1);
  return 0;
}

CRRETURN
CRuntime_init(CRuntime* const restrict self)
{
  TRY(CTP_init(&self->task_pool));

  FOR_EACH_ENGINE_INDEX(i)
  {
    CRESULT_FULL_MATCH(Thread_allocate_memory(),
        res,
        {
          self->engines[i].stack = res;
        },
        {
          UNUSED(res);
          TODO("managet failure in memory allocation, for now panic");
          while(1);
        }
    );
  }

  return OK();
}

CRRETURN
CRuntime_add_task(
    CRuntime* const restrict self,
    const TaskEntry fun,
    void* arg,
    const StackView stack)
{
  CTaskDescription desc ={
    .entry = fun,
    .stack = stack,
    .arg = arg
  };

  TRY(CTP_add_task(&self->task_pool, desc));

  return OK();
}

CRRETURN
CRuntime_start_sync(CRuntime* const restrict self)
{
  ThreadExec entry = {
    .entry = _CS_trampoline,
    .arg = &self->task_pool,
  };
  FOR_EACH_ENGINE_INDEX(i)
  {
    CRESULT_FULL_MATCH(Thread_start(&self->engines[i].stack, entry),
        res,
        {
          self->engines[i].id = res;
        },
        {
          TODO("free allocated stack in star fails");
          return ERR(res.status, res.description);
        }
    );
  }

  FOR_EACH_ENGINE_INDEX(i)
  {
    CRESULT_FULL_MATCH(Thread_wait(self->engines[i].id),
        res,
        {
          UNUSED(res);
          TODO("manage results of engines");
        },
        {
          UNUSED(res);
          TODO("manage err of engines");
        }
    );
  }


  return OK();
}

CRRETURN
CRuntime_terminate(CRuntime* const restrict self)
{
  UNUSED(self);

  TODO();
  return OK();
}

void CRuntime_yield(void* env)
{
  CTask* task = (CTask*) env;
  Context_switch(&task->ctx, task->caller);
}
