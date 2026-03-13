#include "CRuntime.h"

#include <CResult.h>
#include <assert.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>
#include <stdio.h>

char idle_stack[16384]; //FIXME: need an allocator
static int idle_fun(void* input, void* env)
{
  CTaskEnv* c_env = (CTaskEnv*) env;
  UNUSED(input);
  while(1)
  {
    TODO("time delay for idle task");
    printf("idle runtime ctx=%p\n", (void*) c_env->runtime_context);
    Context_switch(c_env->task_context, c_env->runtime_context);
  }
  return 0;
}

static int _Cruntime_schedule(CRuntime* self)
{
  CTask idle_task = {
    .stack = INIT_STATIC_STACK(idle_stack),
    .arg = NULL,
    .entry = idle_fun,
    .runtime_ctx = &self->runtime_context,
  };

  CRESULT_ERR_MATCH(CTP_add_task(&self->task_pool, idle_task),
      Err,{
        UNUSED(Err);
        TODO("manage error in adding idle task");
        assert(0 && "push idle task failed");
      }
  );
  while(1)
  {
    CRESULT_FULL_MATCH(CTP_next(&self->task_pool),
      task,{
        printf("runtime ctx=%p\n", (void*) &self->runtime_context);
        Context_switch(&self->runtime_context, task);
      },
      {
        UNUSED(task);
        if (task.status == CR_STATUS_ERR_QTASK_EMPTY)
        {
        TODO("manage empty task pool, should never e possible but who knows");
        assert(0 && "empty task pool");
        }
      }
    );
  }
}

static int _CRuntime_trampoline(void* const restrict input, void* env)
{
  CRuntime* const restrict self = (CRuntime *) input;
  UNUSED(env);

  if (self) _Cruntime_schedule(self);

  TODO("panic function");
  while(1);

  return 0;
}

CRRETURN
CRuntime_init(CRuntime* const restrict self, const StackView stack)
{
  TRY(Context_init(
        &self->runtime_context,
        stack,
        INIT_TASK_ACTION(
          _CRuntime_trampoline,
          self,
          NULL)));

  TRY(CTP_init(&self->task_pool));

  return OK();
}

CRRETURN
CRuntime_add_task(
    CRuntime* const restrict self,
    const entry fun,
    void* arg,
    const StackView stack)
{
  TRY(CTP_add_task(
        &self->task_pool,
        (CTask){.stack=stack, .entry=fun, .arg=arg, .runtime_ctx=&self->runtime_context}));

  return OK();
}

CRRETURN
CRuntime_start_sync(CRuntime* const restrict self)
{
  Context ctx={0};
  Context_switch(&ctx, &self->runtime_context);
  return ERR(CR_STATUS_ERR_UNREACHABLE_CODE, "CRuntime_start_sync terminated, should not happen");
}

CRRETURN
CRuntime_terminate(CRuntime* const restrict self)
{
  UNUSED(self);

  TODO();
  return OK();
}
