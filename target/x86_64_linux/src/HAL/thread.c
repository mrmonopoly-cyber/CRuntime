#include <CRuntime/common/HAL/thread.h>

#include <assert.h>
#include <pthread.h>

#include <CResult.h>

#include <CRuntime/common/errors/errors.h>
#include <unistd.h>

typedef struct{
  pthread_attr_t attr;
}ThreadMemImp;

static_assert(sizeof(ThreadMemImp) == sizeof(ThreadMem), "sizeof(ThreadMemImp) == sizeof(ThreadMem)");
static_assert(_Alignof(ThreadMemImp) == _Alignof(ThreadMem), "_Alignof(ThreadMemImp) == _Alignof(ThreadMem)");

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

CRReturn Thread_allocate_memory(ThreadMem* const th)
{
  ThreadMemImp* self = (ThreadMemImp *) th;

  if(pthread_attr_init(&self->attr)<0)
  {
    return ERR(CR_STATUS_ERR_UNKNOWN, "pthread_attr_init failed");
  }

  if(pthread_attr_setstacksize(&self->attr, 16384)<0)
  {
    return ERR(CR_STATUS_ERR_UNKNOWN, "pthread_attr_setstacksize failed");
  }

  return OK();
}
#undef ERR_THREAD_I

#define ERR_START(...) \
  CRESULT_T_ERR(ThreadStart,((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(ThreadStart) Thread_start(ThreadMem* const stack, const ThreadExec entry)
{
  ThreadMemImp* self = (ThreadMemImp*) stack;
  pthread_t pth;
  EntryWrapperInput in = {
    .entry = entry.entry,
    .arg = entry.arg,
  };

  if(pthread_create(&pth,&self->attr, _entry_wrapper, &in)<0)
  {
    return ERR_START(CR_STATUS_ERR_UNKNOWN, "pthread_attr_setstacksize failed");
  }
  sleep(1);

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
