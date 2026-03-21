#include "CTP.h"
#include "CRuntime/common/log/log.h"

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

static inline void _drain_executors(CTP* const restrict self)
{
  CSAQPopRes res = {0};

  for(size_t i=0; i<self->num_executors; i++)
  {
    CS* executor = &self->executors[i];
    res = CSAQ_pop_try(&executor->drain_queue);

    while(CRESULT_IS_OK(res))
    {
      CRESULT_ERR_MATCH(CSQ_push_try(&self->waiting_queue, CRESULT_OK_VAL(res)),
          err,{
            TODO("failure in push on waiting_queue, example if full");
            UNUSED(err);
          }
      );
      res=CSAQ_pop_try(&executor->drain_queue);
    }
  }
}

static inline void _process_input_task(CTP* const restrict self)
{
  CTaskDescription* t_in = NULL;
  CTask* p_task= NULL;
  size_t pool_index = 0;
  ContextAction action = {
    .entry = _task_trampoline,
    .arg = NULL,
  };

  for(size_t i=0; i<CTDQ_CAPACITY;i++)
  {
    CRESULT_FULL_MATCH(CTDQ_pop_try(&self->input_tasks_queue),
        res,
        {
          t_in = res;

          while(self->list[pool_index].entry)
          {
            pool_index = (pool_index + 1) % CTP_CAPACITY;
          }
          p_task = &self->list[pool_index];
          p_task->name = t_in->name;
          p_task->type = TaskType_User;

          action.arg = p_task;

          CRESULT_ERR_MATCH(Context_init(&p_task->ctx, t_in->stack, action),
              err,{
                TODO("manage error in contex init fail");
                UNUSED(err);
              }
          );

          p_task->arg = t_in->arg;
          p_task->entry = t_in->entry;
          CRESULT_ERR_MATCH(CSQ_push_try(&self->waiting_queue, p_task),
              err,{
                TODO("failure in push on waiting_queue, example if full");
                UNUSED(err);
              }
          );
        },
        {
          if(res.status != CR_STATUS_ERR_EMPTY)
          {
            TODO("unamanaged error");
            UNUSED(res);
          }
          break;
        }
    );
  }
}

static int _system_task_collect(void* arg)
{
  CTP* self = (CTP*) arg;

  _drain_executors(self);
  _process_input_task(self);

  return 0;
}

static int _system_task_schedule(void* arg)
{
  CTP* self = (CTP*) arg;
  CSQPopRes res = {0};
  CRReturn ret_res ={0};
  CTask* task = NULL;
  size_t index =0;

  res = CSQ_pop_try(&self->waiting_queue);

  while(CRESULT_IS_OK(res))
  {
    task = CRESULT_OK_VAL(res);

    do{
      index = self->exec_cursor;
      ret_res =
        CSAQ_push_try(&self->executors[index].world_task_queue[task->type], task);
      self->exec_cursor = (self->exec_cursor + 1) % self->num_executors;
    }
    while(CRESULT_IS_ERR(ret_res));

    CRESULT_ERR_MATCH(LOG(self->runtime_ctx->logger, CR_MAX_NUM_OF_CORES, Trace,
        "pushing task %s on executor %d, queue %d", task->name, index, task->type),
        err,
        {
          UNUSED(err);
          TODO("manager error in logging");
        }
    );

    res=CSQ_pop_try(&self->waiting_queue);
  }
  return 0;
}

CRRETURN CTP_init(CTP* const restrict self, CS* const executors, const size_t size, CCTX* ctx)
{
  assert(self);

  SystemTaskInfo* system_stack = NULL;
  ContextAction action = {
    .entry = _task_trampoline,
    .arg = NULL,
  };

  cr_memset(self, 0, sizeof(*self));

  TRY(CSQ_init(&self->waiting_queue));
  TRY(CTDQ_init(&self->input_tasks_queue));

  self->system_tasks[SystemTask_collect].task = 
    CTask_init(_system_task_collect, self, "Collector", TaskType_System);

  self->system_tasks[SystemTask_schedule].task = 
    CTask_init(_system_task_schedule, self, "Scheduler", TaskType_System);

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

  self->executors = executors;
  self->num_executors = size;
  self->runtime_ctx = ctx;

  return OK();
}

CRRETURN CTP_add_task(CTP* const restrict self, const CTaskDescription task)
{
  assert(self);
  assert(task.entry);
  assert(task.stack.start_addr);
  assert(task.stack.size);

  if (self->input_tasks_cursor >= CTDQ_CAPACITY)
  {
    return ERR(CR_STATUS_ERR_FULL, "user input task's lists is full");
  }

  CTaskDescription* t_p = &self->input_tasks[self->input_tasks_cursor];
  *t_p = task;

  TRY(CTDQ_push_try(&self->input_tasks_queue, t_p));

  self->input_tasks_cursor = (self->input_tasks_cursor+1) % CTDQ_CAPACITY;

  return OK();
}

CRRETURN CTP_bootstrap(CTP* const restrict self)
{
  TRY(CSAQ_push_try(
      &self->executors->world_task_queue[TaskType_System],
      &self->system_tasks[SystemTask_collect].task));

  while(!atomic_is_lock_free(&self->system_tasks[SystemTask_collect].running));

  TRY(CSAQ_push_try(
      &self->executors->world_task_queue[TaskType_System],
      &self->system_tasks[SystemTask_schedule].task));

  return OK();
}
