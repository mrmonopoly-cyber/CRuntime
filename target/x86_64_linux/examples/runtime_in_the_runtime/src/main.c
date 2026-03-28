#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CRuntime/CRuntime.h>

char task_stack_1[16384]__attribute__((__aligned__(16)));
char task_stack_2[16384]__attribute__((__aligned__(16)));

int meaning_of_life(void* input)
{
  uintptr_t ans = (uintptr_t) input;
  while(1)
  {
    printf("meaning_of_life is: %ld\n", ans);
    sleep(1);
    CRuntime_yield();
  }
}


CRL inner_logger={0};

int runtime_rec(void* input)
{
  UNUSED(input);
  CRuntime runtime = {0};

  CRESULT_ERR_MATCH(CRuntime_init(&runtime,
        .active_cores = 1,
        .logger_init_opt.logger = &inner_logger),
      err,{
        printf("error init CRuntime: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_add_task(
        &runtime,
        "task",
        meaning_of_life,
        (void*)(uintptr_t)42,
        INIT_STATIC_STACK(task_stack_2)),
      err,{
        printf("error add task_f_2: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_start_sync(&runtime),
      err,{
        printf("error start CRuntime: %s\n", err.description);
        return 1;
      }
  );

  return 0;
}

int main(int argc, char** argv)
{
  CRuntime runtime = {0};
  printf("CRuntime started\n");

  size_t active_procs = 1;

  if (argc > 1)
  {
    active_procs = atoi(argv[1]);
  }

  printf("using %lu procs of %d\n",active_procs, CR_MAX_NUM_OF_CORES);

  CRESULT_ERR_MATCH(CRuntime_init(&runtime, .active_cores = active_procs),
      err,{
        printf("error init CRuntime: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_add_task(
        &runtime,
        "runtime",
        runtime_rec,
        NULL,
        INIT_STATIC_STACK(task_stack_1)),
      err,{
        printf("error add task_f_2: %s\n", err.description);
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
