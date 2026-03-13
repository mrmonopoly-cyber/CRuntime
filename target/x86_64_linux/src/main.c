#include "CRuntime/common/HAL/context.h"
#include "CRuntime/common/common.h"
#include <stdio.h>
#include <unistd.h>

#include <CRuntime/CRuntime.h>

char scheduler_stack[16384]__attribute__((__aligned__(16)));
char task_stack[16384]__attribute__((__aligned__(16)));

int task_f(void* in, void* env)
{
  UNUSED(in);
  UNUSED(env);

  while (1)
  {
    printf("hello from the task\n");
    sleep(1);
  }
}


int main(void)
{
  CRuntime runtime = {0};
  printf("CRuntime started\n");

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, INIT_STATIC_STACK(scheduler_stack)),
      err,{
        printf("error init CRuntime: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_add_task(
        &runtime,
        task_f,
        NULL,
        INIT_STATIC_STACK(task_stack)),
      err,{
        printf("error start CRuntime: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_start_sync(&runtime),
      err,{
        printf("error start CRuntime: %s\n", err.description);
        return 1;
      }
  );

  // CRESULT_ERR_MATCH(CRuntime_terminate(&runtime),
  //     err,{
  //       printf("error terminate CRuntime: %s\n", err.description);
  //       return 1;
  //     }
  // );

  printf("CRuntime ended\n");

  return 0;
}
