#include <stdio.h>
#include "CRuntime.h"

int main(void)
{
  CRuntime runtime = {0};

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, INIT_DEFAULT),
      err,{
        printf("error in init of runtime: %s\n", err.description);
        return 1;
      }
  );

  return 0;
}
