#include "CTP.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>

static int _task_trampoline(void* arg1)
{
  CTask* task = (CTask*)arg1;

  if (task)
  {
    task->entry(task->arg);
  }
  task->entry = NULL;
  Context_switch(&task->ctx, task->caller);
  TODO("panic task trampoline");
  while(1);
  return 0;
}

static int _system_task_collect(void* arg)
{
  UNUSED(arg);
  TODO();
  return 0;
}

static int _system_task_schedule(void* arg)
{
  UNUSED(arg);
  TODO();
  return 0;
}

CRRETURN CTP_init(CTP* const restrict self)
{
  assert(self);

  struct __SystemStackInfo* system_stack = NULL;
  ContextAction action = {
    .entry = _task_trampoline,
    .arg = NULL,
  };

  memset(self, 0, sizeof(*self));

  self->system_tasks[SystemTask_collect].task = 
    CTask_init(_system_task_collect, NULL, TaskType_System);

  self->system_tasks[SystemTask_schedule].task = 
    CTask_init(_system_task_schedule, NULL, TaskType_System);

  for(SystemTask t=SystemTask_collect; t<__NUM_SystemTask; t++)
  {
    system_stack = &self->system_tasks[t];

    action.arg = &system_stack->task;
    CRESULT_FULL_MATCH(Thread_allocate_memory(),
        res,{
          system_stack->stack.start_addr = res.low_addr;
          system_stack->stack.size = res.size;
        },
        {
          //FIXME: deallocate the old stack
          TODO("deallocate old memory");
          return ERR(res.status, res.description);
        }
    );
    CRESULT_ERR_MATCH(Context_init(&system_stack->task.ctx, system_stack->stack, action),
        err,{
          //FIXME: deallocate the old stack
          TODO("deallocate old memory");
          return ERR(err.status, err.description);
        }
    );
  }

  return OK();
}

CRRETURN CTP_add_task(CTP* const restrict self, const CTaskDescription task)
{
  assert(self);
  assert(task.entry);
  assert(task.stack.start_addr);
  assert(task.stack.size);

  if (self->input_tasks_cursor >= CTP_MAX_INPUT_TASKS)
  {
    return ERR(CR_STATUS_ERR_FULL, "user input task's lists is full");
  }

  self->input_tasks[self->input_tasks_cursor] = task;
  self->input_tasks_cursor = (self->input_tasks_cursor+1) % CTP_MAX_INPUT_TASKS;

  return OK();
}
