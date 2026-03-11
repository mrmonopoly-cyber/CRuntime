#include <stdio.h>
#include <unistd.h>

#include <CRuntime/CRuntime.h>

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


  CRESULT_ERR_MATCH(CRuntime_start_sync(&runtime),
      err,{
        printf("error start CRuntime: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_terminate(&runtime),
      err,{
        printf("error terminate CRuntime: %s\n", err.description);
        return 1;
      }
  );

  printf("CRuntime ended\n");

  return 0;
}
