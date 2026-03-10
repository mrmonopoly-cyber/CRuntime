#include "CRuntime/common/errors/errors.h"
#include <CRuntime/common/common.h>
#include <CResult.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_OK(pid) CRESULT_T_OK(CRSpawnReturn, (pid))
#define PROC_ERR(...) CRESULT_T_ERR(CRSpawnReturn, ((CRStatus){__VA_ARGS__}))

CRESULT_RETURN(CRSpawnReturn) CRProcess_spawn(const CRProcess proc)
{
  CRPid pid = fork();
  if (pid<0) return PROC_ERR(
      .status = CR_STATUS_ERR_PROCESS,
      .description ="fork failed");

  if (!pid) //INFO: child
  {
    exit(proc.proc_fun(proc.input));
  }

  return PROC_OK(pid);
}

CRRETURN CRProcess_terminate(const CRPid pid)
{
  int err=0;
  char err_msg[128] ={0};

  if((err=kill(pid, SIGKILL))<0)
  {
    snprintf(err_msg, sizeof(err_msg), "failed killing process %d, err: %s", pid, strerror(errno));
    perror(err_msg);
    return ERR(
        .status = CR_STATUS_ERR_PROCESS,
        .description = "failed to kill process");
  }

  return OK();
}

void CRProcess_exit(const int exit_val, const char* msg)
{
  perror(msg);
  exit(exit_val);
}
