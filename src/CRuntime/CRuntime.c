#include "CRuntime.h"
#include "CResult.h"
#include "CRuntime/common/HAL/process.h"
#include "CRuntime/task_pool/task_pool.h"
#include "common/common.h"
#include "common/errors/errors.h"
#include <assert.h>

CRRETURN _CRuntime_init(CRuntime* const self, const CRuntimeInitOpt opt)
{
  UNUSED(opt);

  CRPid task_pool_pid = 0;
  CRESULT_FULL_MATCH(CTask_init(), res,
      task_pool_pid = res,
      return ERR(.status=res.status,.description=res.description);
      );

  assert(task_pool_pid);
  self->task_pool_pid = task_pool_pid;

  return OK();
}

CRRETURN CRuntime_terminate(CRuntime* const self)
{
  if (self->task_pool_pid)
  {
    CRESULT_ERR_MATCH(CRProcess_terminate(self->task_pool_pid),
        err, return ERR(err.status,err.description));
  }

  return OK();
}
