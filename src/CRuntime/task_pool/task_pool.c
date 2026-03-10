#include "task_pool.h"
#include "CResult.h"
#include "CVector.h"

#include <CRuntime/common/common.h>
#include <CRuntime/common/HAL/process.h>

#define TPI_ERR(...) CRESULT_T_ERR(CTInitRet, ((CRStatus){__VA_ARGS__}))
#define TPI_OK(pid) CRESULT_T_OK(CTInitRet, (pid))

typedef C_VECTOR_TEMPLATE(CTask) CTaskVector;
typedef struct{
  CTaskVector task_pool;
}CTaskPool;


static void* _task_loop(void* input)
{
  UNUSED(input);

  return NULL;
}

CRESULT_RETURN(CTInitRet) CTask_init(void)
{
  CRPid task_pool_pid = 0;

  CRESULT_FULL_MATCH(CRProcess_spawn((CRProcess){.input = NULL, .proc_fun = _task_loop}),
      res,
      task_pool_pid = res,
      return TPI_ERR(.status = res.status, .description = res.description);
      );

  return TPI_OK(task_pool_pid);
}
