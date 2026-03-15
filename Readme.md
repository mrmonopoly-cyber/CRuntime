# CRuntime

**CRuntime** is a Scheduler (cooperate at the moment, but it will ha preemption in the future)
which is able to run task concurrently by distributing them on **N** virtual cores. 
This cores can be anything from system level thread to real cpu core.
Each core independently chooses which tasks to run among the one which has been assigned to it.
The library is implemented in **C11**, and it does not depend on any external library not even 
on **glibc**.
When some functions of the **glibc** are used, they have been forward declared to let the user
implement them as they like.
Also a **HAL** has been designed to facilitate the porting on different platform.
Thanks to all of these the library if completely **freestanding** using **C11**.

> [!IMPORTANT]
> At the current moment a few extensions of **gcc** are used in particular `__typeof__`.
> They will be removed in the future but at the moment they are in use.
> Check that your C compiler supports them.

# Usage

Below an example on how to use the library:

> [!NOTE]
> The following example assumes that a valid deployment is available

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CRuntime/CRuntime.h>

char task_stack_1[16384]__attribute__((__aligned__(16)));
char task_stack_2[16384]__attribute__((__aligned__(16)));

int task_f_1(void* in, void* env)
{
  UNUSED(in);
  UNUSED(env);

  for(int a=0;a<5;a++)
  {
    printf("hello from the task 1\n");
    CRuntime_yield(env);
    sleep(1);
  }

  return 0;
}

int task_f_2(void* in, void* env)
{
  UNUSED(in);
  UNUSED(env);

  for(int a=0;a<5;a++)
  {
    printf("hello from the task 2\n");
    CRuntime_yield(env);
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
        NULL,
        INIT_STATIC_STACK(task_stack_1)),
      err,{
        printf("error add task_f_1: %s\n", err.description);
        return 1;
      }
  );

  CRESULT_ERR_MATCH(CRuntime_add_task(
        &runtime,
        task_f_2,
        NULL,
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


```

In this example the program initialize the runtime and the add two tasks to it before starting.
The two tasks are labeled `task_f_2` and `task_f_1`.
The Scheduler then executes the two task alternating all of them and waits.
> [!IMPORTANT]
> The Scheduler is in active development and this behaviour may change.

Here is the output:

```txt

CRuntime started
using 1 procs of 2
hello from the task 1
hello from the task 2
hello from the task 1
hello from the task 2
hello from the task 1
hello from the task 2
hello from the task 1
hello from the task 2
hello from the task 1
hello from the task 2


```

# Supports

Here a list of the supported target at the moment:

> [!NOTE]
> At the current stage of the library the supports for many targets is not fundamental,
> for this reason the support is pretty limited and mostly used in the deployment.

| Target        | Link                                          |
|---------------|-----------------------------------------------|
| x86_64_linux  | [Deployment](./target/x86_64_linux/Readme.md) |

To deploy the library on another target the only you only need to implement a few functions
in your sources and compile it with the library.
All the functions are specified in the HAL.
Also, some global macros are used to configure the system. Most of them have defaults but when
it's not possible a compile error with a message is given to the user so that it's easier to 
understand what is the problem.

# Documentation

For a more maintainable Documentation, it is kept in the source code of the project
and [Doxygen](https://www.doxygen.nl/) it's used to generate the doc itself.

to generate the doc run the following command:

```sh

doxygen Doxyfile

```

This command will generate all the files in `doc/doxygen`.
If you have some doubts you should check the doc since it's where most of the details have been
written. Also, outside **Doxygen **, no internet connection is needed to compile or research the
Documentation. 
