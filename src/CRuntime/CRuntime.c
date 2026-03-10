#include "CRuntime.h"
#include "CResult.h"
#include "CRuntime/common/HAL/process.h"
#include "CRuntime/task_pool/task_pool.h"
#include "common/common.h"
#include "common/errors/errors.h"

CRRETURN _CRuntime_init(CRuntime* const self, const CRuntimeInitOpt opt)
{
  UNUSED(opt);

  CRPid task_pool_pid = 0;
  CRESULT_FULL_MATCH(CTask_init(), res,
      task_pool_pid = res,
      return ERR(.status=res.status,.description=res.description);
      );

  if (!task_pool_pid) //INFO: child process terminated
  {
    CRProcess_exit(0, "task pool exited");
  }

  self->task_pool_pid = task_pool_pid;

  return OK();
}

CRRETURN CRuntime_terminate(CRuntime* const self)
{
  CRESULT_ERR_MATCH(CRProcess_terminate(self->task_pool_pid),
      err, return ERR(err.status,err.description));

  return OK();
}
