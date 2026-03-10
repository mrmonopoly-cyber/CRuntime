#include <stdio.h>
#include <CRuntime/CRuntime.h>

int main(void)
{
  CRuntime runtime = {0};

  printf("CRuntime started\n");

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, INIT_DEFAULT),
      err,{
        printf("error in init of runtime: %s\n", err.description);
        return 1;
      }
  );

  printf("CRuntime ended\n");

  return 0;
}
