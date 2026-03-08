#include <stdio.h>
#include "CRuntime.h"

int main(void)
{
  CRuntime runtime = {0};

  ERR_MATCH(CRuntime_init(&runtime),err,{
    printf("error in init of runtime: %s\n", err.description);
    return 1;
  });

  printf("hello, world!\n");
  return 0;
}
