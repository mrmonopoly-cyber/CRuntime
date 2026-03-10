#include <stdio.h>
#include <CRuntime/CRuntime.h>
#include <unistd.h>

int main(void)
{
  CRuntime runtime = {0};

  printf("CRuntime started\n");

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, INIT_DEFAULT),
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
