#include "CScheduler.h"
#include "CRuntime/common/HAL/debug.h"
#include "CRuntime/common/utils/utils.h"

#include <assert.h>
#include <stdatomic.h>

#include <CRuntime/common/log/log.h>

#define CHECK_CS_INPUT(cs) \
  if (!cs) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CS_init: cs ptr is null")

CRRETURN CS_init(CS* const self, const size_t worker_id, CCTX* cr_ctx)
{
  assert(self);

  cr_memset(self, 0, sizeof(*self));

  self->cr_ctx = cr_ctx;

  TRY(CSAQ_init(&self->drain_queue));
  TRY(CSQ_init(&self->local_queue));
  for(size_t i=0; i<__NUM_TaskType; i++)
  {
    TRY(CSAQ_init(&self->world_task_queue[i]));
  }

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
    CRESULT_OK_MATCH(CSQ_push_try(&self->local_queue, task),
        ok,{
          UNUSED(ok);
          return;
        }
    );

    if(CRESULT_IS_ERR(LOG(self->cr_ctx->logger, self->worker_id, Trace,
          "worker: %d pushing task %s on drain queue", self->worker_id, task->name, NULL)))
    {
          TODO("unamanager log failure");
    }

    CRESULT_ERR_MATCH(CSAQ_push_try(&self->drain_queue, task),
        err,{
          UNUSED(err);
          TODO("unable to insert the task nor in the local_queue nor in drain_queue");
        }
    );
  }
}

static inline void _schedule_task(CS* const self, CTask* const task, const bool local)
{
    CRESULT_ERR_MATCH(LOG(
          self->cr_ctx->logger,
          self->worker_id,
          Trace,
          "worker %d: scheduling %s task type: %d task: %s",
          self->worker_id, local ? "local" : "", task->type, task->name,
          NULL
          ),
        err,
        {
          TODO("unamanager log failure");
          UNUSED(err);
        }
    );
    _CS_manage_task(self, task);
}

CRRETURN CS_run(CS* const restrict self)
{
  assert(self);
  bool found=false;

  while (true)
  {
    CRESULT_OK_MATCH(CSAQ_pop_try(&self->world_task_queue[TaskType_System]),
        res,{
         _schedule_task(self, res, false);
         continue;
        }
    );

    if(CRLog_size(self->cr_ctx->logger) > CR_LOG_FILL_THRESHOLD)
    {
      CRLog_drain_x(self->cr_ctx->logger, CR_LOG_FILL_THRESHOLD);
    }

    CRESULT_OK_MATCH(CSQ_pop_try(&self->local_queue),
        res,{
         _schedule_task(self, res, true);
         continue;
        }
    );

    for(TaskType t=TaskType_System + 1; t<__NUM_TaskType; t++)
    {
      CRESULT_OK_MATCH(CSAQ_pop_try(&self->world_task_queue[t]),
          res,{
           _schedule_task(self, res, false);
           found = true;
           continue;
          }
      );
    }

    if(found) continue;

    CRESULT_ERR_MATCH(LOG(
          self->cr_ctx->logger,
          self->worker_id,
          Trace,
          "worker: %d scheduling idle task",
          self->worker_id, 
          NULL
          ),
        err,
        {
          TODO("unamanager log failure");
          UNUSED(err);
        }
    );


    //INFO: idle task
    CRLog_drain_x(self->cr_ctx->logger, 1); //TODO: test best amount to drain. Need measurements
  }

  return OK();
}
