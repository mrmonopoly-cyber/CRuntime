#include "CRuntime.h"
#include "CRuntime/common/errors/errors.h"

#include <assert.h>
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/common.h>

typedef struct {
  CTP task_pool;
  struct{
    StackInfo stack;
    ThreadId id;
  }engines[CR_MAX_NUM_OF_CORES];
}CRuntimeImp;

static_assert(sizeof(CRuntimeImp)==sizeof(CRuntime), "sizeof(CRuntimeImp)==sizeof(CRuntime)");
static_assert(_Alignof(CRuntimeImp)==_Alignof(CRuntime), "_Alignof(CRuntimeImp)==_Alignof(CRuntime)");

#define CHECK_SELF_INPUT(self) \
  if(!(self)) return ERR(CR_STATUS_ERR_INVALID_INPUT, "CRuntime input ptr is NULL");

int _CS_trampoline(void* in)
{
  CS cs ={0};
  CTP* ctp = (CTP*)in;

  if (ctp) {
    CRESULT_ERR_MATCH(CS_init(&cs,ctp),
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

CRRETURN _CRuntime_init(CRuntime* const restrict cr, const CRuntimeInitOpt opt)
{
  CRuntimeImp* self = (CRuntimeImp*)cr;

  CHECK_SELF_INPUT(self);

  if (opt.active_cores > CR_MAX_NUM_OF_CORES)
  {
    return ERR(CR_STATUS_ERR_INVALID_INPUT, "active cores is bigger than CR_MAX_NUM_OF_CORES");
  }

  TRY(CTP_init(&self->task_pool));

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
    CRuntime* const restrict cr,
    const TaskEntry fun,
    void* arg,
    const StackView stack)
{
  CRuntimeImp* self = (CRuntimeImp*)cr;
  CTaskDescription desc ={
    .entry = fun,
    .stack = stack,
    .arg = arg
  };
  CHECK_SELF_INPUT(self);

  TRY(CTP_add_task(&self->task_pool, desc));

  return OK();
}

CRRETURN CRuntime_start_sync(CRuntime* const restrict cr)
{
  CRuntimeImp* self = (CRuntimeImp*)cr;
  ThreadExec entry = {
    .entry = _CS_trampoline,
    .arg = &self->task_pool,
  };
  CHECK_SELF_INPUT(self);

  for(int i=0;i<CR_MAX_NUM_OF_CORES;i++)
  {
    if (self->engines[i].stack.low_addr == NULL)
    {
      //INFO: skip if not really initialized, may be an error or the core has been activated
      continue;
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
      //INFO: skip if not really initialized, may be an error or the core has been activated
      continue;
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

void CRuntime_yield(void* env)
{
  CTask* task = (CTask*) env;
  Context_switch(&task->ctx, task->caller);
}
