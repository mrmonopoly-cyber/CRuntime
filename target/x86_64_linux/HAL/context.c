#include <CRuntime/common/HAL/context.h>

#include <stdio.h>
#include <assert.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>
#include <string.h>

typedef uint64_t Reg;
typedef struct{
  Reg rsp;
}CpuState;

struct __ContextImp{
  CpuState __cpu_state;
  StackView __stack;
  ContextAction __action;
}ALIGNED_(CR_CONTEXT_ALIGNEMENT);

static_assert(sizeof(struct __ContextImp) == sizeof(Context), "context types differ in sizes");
static_assert(_Alignof(struct __ContextImp) == _Alignof(Context), "context types differ in alignement");


static void _panic(void) __attribute__((noreturn));
static void _panic(void)
{
  printf("context panic\n");
  while(1);
}

static void _task_trampoline(void)
{
  struct __ContextImp *ctx = NULL;

  __asm__ volatile("mov %%r15, %0": "=r" (ctx));

  if (ctx) {
    ctx->__action.entry(ctx->__action.arg);
  }
  TODO("panic");
  while(1);
}


CRRETURN Context_init(Context* const restrict cs,
    const StackView stack,
    const ContextAction action)
{
  struct __ContextImp* ctx = (struct __ContextImp*)cs;
  uintptr_t sp_addr = 0;
  void** sp = NULL;

  memset(ctx, 0, sizeof(*ctx));
  ctx->__stack = stack;
  ctx->__action = action;

  sp_addr = (uintptr_t) ((char*)stack.start_addr + stack.size);
  sp_addr = sp_addr & ~0xF; //rsp % 16 == 8

  sp = (void**) sp_addr;

  *(--sp) = (void*) (uintptr_t) _panic;
  *(--sp) = (void*) (uintptr_t) _task_trampoline;
  *(--sp) = 0;            //rbx
  *(--sp) = 0;            //rbp
  *(--sp) = 0;            //r11
  *(--sp) = 0;            //r12
  *(--sp) = 0;            //r13
  *(--sp) = 0;            //r14
  *(--sp) = ctx;          //r15

  ctx->__cpu_state.rsp = (Reg) sp;

  return OK();
}

void __attribute__((__naked__)) Context_switch(
    struct __Context* const restrict old_cs __attribute__((__unused__)),
    const Context* const restrict new_cs __attribute__((__unused__)))
{
    __asm__ volatile(
      "push %rbx\n\r"
      "push %rbp\n\r"
      "push %r11\n\r"
      "push %r12\n\r"
      "push %r13\n\r"
      "push %r14\n\r"
      "push %r15\n\r"

      "mov %rsp, (%rdi)\n\r"
      "mov (%rsi), %rsp\n\r"

      "pop %r15\n\r" 
      "pop %r14\n\r"
      "pop %r13\n\r"
      "pop %r12\n\r"
      "pop %r11\n\r"
      "pop %rbp\n\r"
      "pop %rbx\n\r"

      "ret"
      );
}

/*
typedef struct{
  Context ctx;  -> rdi //size 48, offset 0
  Context* caller; -> rsi //size 8, offset 48
  TaskEntry entry;
}CTask;
 */
__attribute__((__naked__))
void yield_real(void)
{
    __asm__ inline(
        "mov %r15, %rdi\n\r"
        "mov 48(%r15), %rsi\n\t"  /* rsi = env->caller */
        "call Context_switch\n\r"
        "ret\n\r"
        );
}
