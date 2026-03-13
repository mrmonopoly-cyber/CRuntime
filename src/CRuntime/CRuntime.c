#include "CRuntime.h"
#include "CRuntime/common/HAL/context.h"

#include <CResult.h>
#include <assert.h>

#include <CRuntime/CScheduler/CScheduler.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>
#include <stddef.h>

int _CS_trampoline(void* in, void* env)
{
  CTP* ctp = (CTP*)in;
  UNUSED(env);

  if (ctp) {
    CS_init(ctp);
  }

  TODO("panic");
  while(1);
  return 0;
}

CRRETURN
CRuntime_init(CRuntime* const restrict self, const StackView stack) //FIX: better cs stacks
{
  TRY(CTP_init(&self->task_pool));

  TaskAction action ={
    .entry = _CS_trampoline,
    .arg = &self->task_pool,
    .env = NULL,
  };

  TRY(Context_init(
        &self->engines[0],
        stack,
        action));

  return OK();
}

CRRETURN
CRuntime_add_task(
    CRuntime* const restrict self,
    const entry fun,
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
  Context_switch(&self->CR_context, &self->engines[0]);
  return OK();
}

CRRETURN
CRuntime_terminate(CRuntime* const restrict self)
{
  UNUSED(self);

  TODO();
  return OK();
}
