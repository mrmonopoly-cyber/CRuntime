#include "CRuntime.h"

#include <CResult.h>
#include <assert.h>

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/HAL/stack.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>

typedef struct{
  Context* task_context;
  const taskAction entry;
  void* const input;
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
  int status = t_in.entry(t_in.input);
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
    .input = NULL,
    .entry = idle_fun,
  };

  CRESULT_ERR_MATCH(Context_init(&idle_task,
        _CRuntime_task_wrapper, &in, CRStackInit(idle_stack, sizeof(idle_stack))),
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
  while(1);

  return 0;
}

CRRETURN
CRuntime_init(CRuntime* const restrict self, const CRStack stack)
{
  TRY(Context_init(
      &self->runtime_context,
      _CRuntime_trampoline,
      self,
      stack));

  TRY(CTP_init(&self->task_pool));

  return OK();
}

CRRETURN
CRuntime_add_task(CRuntime* const restrict self,
    const taskAction entry, void* input, const CRStack stack)
{
  typedef CTaskTemplate(TaskWrapperIN, 1) CTaskWrapper;

  CTaskWrapper task = {
    .stack = stack,
    .entry = _CRuntime_task_wrapper,
    .input_size = sizeof(TaskWrapperIN),
    .input = 
    {
      {
        .entry = entry,
        .input = input,
        .self = self,
        .task_context = NULL,
      }
    },
  };

  CTP_add_task(&self->task_pool, &task);

  return OK();
}

CRRETURN
CRuntime_start_sync(CRuntime* const restrict self)
{
  Context_switch(NULL, &self->runtime_context);
  return OK();
}

CRRETURN
CRuntime_terminate(CRuntime* const restrict self)
{
  UNUSED(self);

  TODO();
  return OK();
}
