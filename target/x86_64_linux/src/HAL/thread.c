#include <CRuntime/common/HAL/thread.h>

#include <assert.h>
#include <pthread.h>
#include <sys/mman.h>

#include <CResult.h>

#include <CRuntime/common/errors/errors.h>
#include <unistd.h>

typedef struct{
  ThreadEntry entry;
  void* arg;
}EntryWrapperInput;

void* _entry_wrapper(void* wp)
{
  if(wp)
  {
    EntryWrapperInput self = * (EntryWrapperInput*) wp;
    self.entry(self.arg);
  }
  return NULL;
}

#define OK_AM(...) CRESULT_T_OK(ThreadAM, ((StackInfo){__VA_ARGS__}))
#define ERR_AM(...) CRESULT_T_ERR(ThreadAM, ((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(ThreadAM) Thread_allocate_memory(void)
{
  void *stack = mmap(NULL, 16384, PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_SHARED, -1, 0);

  if (stack) {
    return OK_AM(stack, 16384);
  }else{
    return ERR_AM(CR_STATUS_ERR_UNKNOWN, "nmap failed");
  }

}
#undef OK_AM
#undef ERR_AM

#define ERR_START(...) \
  CRESULT_T_ERR(ThreadStart,((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(ThreadStart) Thread_start(StackInfo* const stack, const ThreadExec entry)
{
  pthread_attr_t ptAttr;
  pthread_t pth;
  EntryWrapperInput in = {
    .entry = entry.entry,
    .arg = entry.arg,
  };

  if(pthread_attr_init(&ptAttr)<0)
  {
    return ERR_START(CR_STATUS_ERR_UNKNOWN, "pthread_attr_init failed");
  }

  if(pthread_attr_setstack(&ptAttr, stack->low_addr, stack->size)<0)
  {
    return ERR_START(CR_STATUS_ERR_UNKNOWN, "pthread_attr_setstack failed");
  }

  if(pthread_create(&pth,&ptAttr, _entry_wrapper, &in)<0)
  {
    return ERR_START(CR_STATUS_ERR_UNKNOWN, "pthread_attr_setstacksize failed");
  }
  sleep(1); //HACK: for now

  return CRESULT_T_OK(ThreadStart, pth);
}
#undef ERR_START

#define ERR_WAIT(...) \
  CRESULT_T_ERR(ThreadWait,((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(ThreadWait) Thread_wait(ThreadId const self)
{
  void * res = NULL;
  if(pthread_join(self, &res)<0)
  {
    return ERR_WAIT(CR_STATUS_ERR_UNKNOWN, "pthread_join failed");
  }

  return CRESULT_T_OK(ThreadWait, (uintptr_t) res);
}
#undef ERR_WAIT
