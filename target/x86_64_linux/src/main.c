#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CRuntime/CRuntime.h>

char task_stack_1[16384]__attribute__((__aligned__(16)));
char task_stack_2[16384]__attribute__((__aligned__(16)));

int task_f_1(void* in)
{
  int answer = (int)(uintptr_t)in;

  UNUSED(in);

  for(int a=0;a<5;a++)
  {
    printf("the answer from task 1 is: %d\n", answer);
    CRuntime_yield();
    sleep(1);
  }

  return 0;
}

int task_f_2(void* in)
{
  int answer = (int)(uintptr_t)in;

  UNUSED(in);

  for(int a=0;a<5;a++)
  {
    printf("the answer from task 1 is: %d\n", answer);
    CRuntime_yield();
    sleep(1);
  }

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
        task_f_1,
        (void*)(uintptr_t)42,
        INIT_STATIC_STACK(task_stack_1)),
      err,{
        printf("error add task_f_1: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_add_task(
        &runtime,
        task_f_2,
        (void*)(uintptr_t)69,
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

  CRESULT_ERR_MATCH(CRuntime_terminate(&runtime),
      err,{
        printf("error terminate CRuntime: %s\n", err.description);
        return 1;
      }
  );

  printf("CRuntime ended\n");

  return 0;
}
