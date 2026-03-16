#include "CRuntime.h"
#include "CRuntime/common/errors/errors.h"

#include <assert.h>
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/common.h>

#define CHECK_SELF_INPUT(self) \
  if(!(self)) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CRuntime input ptr is NULL");

int _CS_trampoline(void* in)
{
  CS cs ={0};
  CSQ* csq = (CSQ*)in;

  if (csq) {
    CRESULT_ERR_MATCH(CS_init(&cs,csq),
        res,{
          TODO("use the error msg in case of failure of CS_init");
          return -res.status;
        }
    );

    CRESULT_ERR_MATCH(CS_run(&cs),
        res,{
          TODO("use the error msg in case of failure of CS_run");
          return -res.status;
        }
    );
  }

  TODO("_CS_trampoline: panic");
  while(1);
  return 0;
}

CRRETURN _CRuntime_init(CRuntime* const restrict self, const CRuntimeInitOpt opt)
{
  CHECK_SELF_INPUT(self);

  if (opt.active_cores > CR_MAX_NUM_OF_CORES)
  {
    return ERR(CR_STATUS_ERR_INVALID_INPUT, "active cores is bigger than CR_MAX_NUM_OF_CORES");
  }else if(opt.active_cores == 0)
  {
    return ERR(CR_STATUS_ERR_INVALID_INPUT, "active cores is must be > 0");
  }

  TRY(CTP_init(&self->task_pool, opt.active_cores));

  for(size_t i=0;i<opt.active_cores;i++)
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

CRRETURN CRuntime_add_task(
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
  CHECK_SELF_INPUT(self);

  TRY(CTP_add_task(&self->task_pool, desc));

  return OK();
}

CRRETURN CRuntime_start_sync(CRuntime* const restrict self)
{
  ThreadExec entry = {
    .entry = _CS_trampoline,
    .arg = NULL,
  };
  CHECK_SELF_INPUT(self);

  for(int i=0;i<CR_MAX_NUM_OF_CORES;i++)
  {
    entry.arg = &self->task_pool.exec_queue[i];
    if (self->engines[i].stack.low_addr == NULL)
    {
      //INFO: skip if not really initialized, may be an error or the core has not been activated
      break;
    }
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

  for(int i=0;i<CR_MAX_NUM_OF_CORES;i++)
  {
    if (self->engines[i].stack.low_addr == NULL)
    {
      //INFO: skip if not really initialized, may be an error or the core has not been activated
      break;
    }
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
