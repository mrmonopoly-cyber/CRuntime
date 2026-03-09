#include "process.h"
#include "CResult.h"
#include "errors/errors.h"

#define PROC_OK(pid) CRESULT_T_OK(CRSpawnReturn, (pid))
#define PROC_ERR(...) CRESULT_T_ERR(CRSpawnReturn, ((CRStatus){__VA_ARGS__}))

#if defined (__linux__) || defined (__gnu_linux__)

#include <unistd.h>

CRESULT_RETURN(CRSpawnReturn) create_process(const CRProcess proc)
{
  CRPid pid = fork();
  if (pid<0) return PROC_ERR(
      .status = CR_STATUS_ERR_PROCESS_SPAWN,
      .description ="fork failed");

  if (!pid) //INFO: child
  {
    proc.proc_fun(proc.input);
  }

  return PROC_OK(pid);
}

#else

#error "platform not supported yet"

#endif /* if defined (__linux__) || defined () */
