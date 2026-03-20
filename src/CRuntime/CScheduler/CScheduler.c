#include "CScheduler.h"
#include "CRuntime/common/utils/utils.h"

#include <assert.h>
#include <stdatomic.h>

#include <CRuntime/common/log/log.h>

#define CHECK_CS_INPUT(cs) \
  if (!cs) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CS_init: cs ptr is null")

int idle_task(void* in)
{
  CS* self= (CS*) in;

  while (self)
  {
    CRLog_drain_x(self->cr_ctx->logger, 1); //TODO: test best amount to drain. Need measurements
    Context_switch(self->active_ctx, &self->ctx);
  }

  TODO("idle_task: panic");
  while(1);
}

CRRETURN CS_init(CS* const self, const size_t worker_id, CCTX* cr_ctx)
{
  ContextAction action ={0};

  assert(self);

  cr_memset(self, 0, sizeof(*self));

  self->cr_ctx = cr_ctx;

  TRY(CVAQ_init(&self->drain_queue));
  TRY(CVQ_init(&self->local_queue));
  for(size_t i=0; i<__NUM_TaskType; i++)
  {
    TRY(CVAQ_init(&self->world_task_queue[i]));
  }

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

  self->worker_id = worker_id;

  return OK();
}

static
void _CS_manage_task(CS* const restrict self, CTask* const task)
{
  task->caller = &self->ctx;
  self->active_ctx = &task->ctx;
  Context_switch(&self->ctx, self->active_ctx);

  //INFO: checking if task yield or terminated.
  if (task && task->entry)
  {
    CRESULT_OK_MATCH(CVQ_push_try(&self->local_queue, task),
        ok,{
          UNUSED(ok);
          return;
        }
    );

    CRESULT_ERR_MATCH(CVAQ_push_try(&self->drain_queue, task),
        err,{
          UNUSED(err);
          TODO("unable to insert the task nor in the local_queue nor in drain_queue");
        }
    );
  }
}

CRRETURN CS_run(CS* const restrict self)
{
  assert(self);
  CTask* task = NULL;

  while (1)
  {
    CRESULT_OK_MATCH(CVAQ_pop_try(&self->world_task_queue[TaskType_System]),
        res,{
          task = res;
          CRESULT_ERR_MATCH(LOG(
                self->cr_ctx->logger,
                self->worker_id,
                Trace,
                "scheduling task type: %d task: %s", TaskType_System, task->name,
                NULL
                ),
              err,
              {
                TODO("unamanager log failure");
                UNUSED(err);
              }
          );
          _CS_manage_task(self, task);
          task = NULL;
          continue;
        }
    );

    CRESULT_OK_MATCH(CVQ_pop_try(&self->local_queue),
        res,{
          task = res;
          CRESULT_ERR_MATCH(LOG(
                self->cr_ctx->logger,
                self->worker_id,
                Trace,
                "scheduling local task: %s", task->name,
                NULL
                ),
              err,
              {
                TODO("unmanaged log failure");
                UNUSED(err);
              }
          );
          _CS_manage_task(self, res);
          task = NULL;
          continue;
        }
    );

    for(TaskType t=TaskType_System + 1; t<__NUM_TaskType; t++)
    {

      CRESULT_OK_MATCH(CVAQ_pop_try(&self->world_task_queue[t]),
          res,{
            task = res;
            CRESULT_ERR_MATCH(LOG(
                  self->cr_ctx->logger,
                  self->worker_id,
                  Trace,
                  "scheduling task type: %d task: %s", t, task->name,
                  NULL
                  ),
                err,
                {
                  TODO("unamanager log failure");
                  UNUSED(err);
                }
            );

            _CS_manage_task(self, task);
            continue;
          }
      );
      task = NULL;
    }

    self->active_ctx = &self->idle_ctx;
    Context_switch(&self->ctx, self->active_ctx);
  }

  return OK();
}
