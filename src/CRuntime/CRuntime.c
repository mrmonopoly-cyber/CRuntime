#include "CRuntime.h"

#include <CResult.h>
#include <assert.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>

typedef struct{
  Context* task_context;
  const TaskAction action;
  CRuntime *const self;
}TaskWrapperIN;

char idle_stack[256];
static int idle_fun(void* input)
{
  UNUSED(input);
  return 0;
}

static int
_CRuntime_task_wrapper(void* input)
{
  Context terminating_context;
  const TaskWrapperIN t_in = *(TaskWrapperIN*) input;
  int status = t_in.action.entry(t_in.action.arg);
  UNUSED(status);
  Context_switch(&terminating_context, &t_in.self->runtime_context);
  
  assert(0 && "unreachable");
  return 0;
}

static void
_CRuntime_schedule_next(CRuntime* const restrict self)
{
  Context idle_task ={0};
  TaskWrapperIN in ={
    .self = self,
    .action = {
      .entry =idle_fun,
      .arg = NULL,
    },
  };

  CRESULT_ERR_MATCH(Context_init(&idle_task,
        (StackView){.size = sizeof(idle_stack), .start_addr = &idle_stack},
        (TaskAction){.entry=_CRuntime_task_wrapper, &in}),
      err,{
        UNUSED(err);
        while(1) TODO("context init failed"); //FIXME: better error handling
      });

  while(1)
  {
    CRESULT_FULL_MATCH(CTP_next(&self->task_pool),
      task,{
        Context_switch(&self->runtime_context, task);
      },
      {
        if(task.status != CR_STATUS_ERR_QTASK_EMPTY)
        {
          //TODO: manage errros
        }
      }
    );
  }
}

static int 
_CRuntime_trampoline(void* const restrict input)
{
  CRuntime* const restrict self = (CRuntime *) input;
  UNUSED(self);
  _CRuntime_schedule_next(self);
  while(1); //TODO: add a panic

  return 0;
}

CRRETURN
CRuntime_init(CRuntime* const restrict self, const StackView stack)
{
  TRY(Context_init(&self->runtime_context,stack,
        (TaskAction){.entry=_CRuntime_trampoline,.arg=self}));

  TRY(CTP_init(&self->task_pool));

  return OK();
}

CRRETURN
CRuntime_add_task(CRuntime* const restrict self, const TaskAction action, const StackView stack)
{
  TRY(CTP_add_task(&self->task_pool, .stack=stack, .action=action));

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
