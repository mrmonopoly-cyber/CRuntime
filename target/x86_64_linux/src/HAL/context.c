#include <CRuntime/common/HAL/context.h>

#include <stdio.h>
#include <assert.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>

struct __ContextImp{
  char data[CR_CONTEXT_SIZE];
}ALIGNED_(CR_CONTEXT_ALIGNEMENT);

static_assert(sizeof(struct __ContextImp) == sizeof(Context), "context types differ in sizes");
static_assert(_Alignof(struct __ContextImp) == _Alignof(Context), "context types differ in alignement");


CRRETURN Context_init(Context* const restrict cs,
    taskAction entry,
    void* input,
    const CRStack stack)
{
  UNUSED(cs);
  union{
    taskAction t;
    void* p;
  }conv ={entry};
  printf("entry: %p\n",conv.p);
  printf("input: %p\n",(void*)input);
  printf("stack: ptr%p, size: %ld\n",stack.buf, stack.size);
  return OK();
}

void Context_switch(Context* const restrict old_cs, const Context* const restrict new_cs)
{
  UNUSED(old_cs);
  UNUSED(new_cs);
  printf("context switchinggg\n");
}
