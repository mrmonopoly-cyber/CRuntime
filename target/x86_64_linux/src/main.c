#include "CRuntime/common/HAL/context.h"
#include <stdio.h>
#include <CRuntime/CRuntime.h>
#include <unistd.h>

char scheduler_stack[1024];

int main(void)
{
  CRuntime runtime = {0};
  printf("CRuntime started\n");

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, CRStackInit(scheduler_stack, sizeof(scheduler_stack))),
      err,{
        printf("error init CRuntime: %s\n", err.description);
        return 1;
      }
  );

  sleep(4);

  CRESULT_ERR_MATCH(CRuntime_terminate(&runtime),
      err,{
        printf("error terminate CRuntime: %s\n", err.description);
        return 1;
      }
  );

  printf("CRuntime ended\n");

  return 0;
}
