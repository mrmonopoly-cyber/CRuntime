#include <CRuntime/common/HAL/context.h>

#include <stdio.h>
#include <assert.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>

typedef uint64_t Reg;
typedef struct{
  Reg rbx;
  Reg rsp;
  Reg rbp;
  Reg r12;
  Reg r13;
  Reg r14;
  Reg r15;

  Reg rdi;
  Reg rsi;
}CpuState;

struct __ContextImp{
  CpuState __cpu_state;
  StackView __stack;
  TaskAction __action;
}ALIGNED_(CR_CONTEXT_ALIGNEMENT);

static_assert(sizeof(struct __ContextImp) == sizeof(Context), "context types differ in sizes");
static_assert(_Alignof(struct __ContextImp) == _Alignof(Context), "context types differ in alignement");


static void _panic(void) __attribute__((noreturn));
static void _panic(void)
{
  printf("panic\n");
  while(1);
}

static void _task_trampoline(Context* cs, void* env)
{
  struct __ContextImp *ctx = (struct __ContextImp*)cs;
  printf("x86_64 context driver trampoline\n");
  if (ctx) {
    ctx->__action.entry(ctx->__action.arg, env);
  }
}


CRRETURN Context_init(Context* const restrict cs,
    const StackView stack,
    const TaskAction action)
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

  ctx->__cpu_state.rsp = (Reg) sp;
  ctx->__cpu_state.rdi = (Reg) ctx;
  ctx->__cpu_state.rsi = (Reg) action.env;


  return OK();
}

void Context_switch(
    Context* const restrict old_cs __attribute__((__unused__)),
    const Context* const restrict new_cs __attribute__((__unused__)))
{
    __asm__ volatile(
      "mov %%rbx,  0(%%rdi)\n"
      "mov %%rsp,  8(%%rdi)\n"
      "mov %%rbp, 16(%%rdi)\n"
      "mov %%r12, 24(%%rdi)\n"
      "mov %%r13, 32(%%rdi)\n"
      "mov %%r14, 40(%%rdi)\n"
      "mov %%r15, 48(%%rdi)\n"
      "mov %%rdi, 56(%%rdi)\n"
      "mov %%rsi, 64(%%rdi)\n"

      "mov  0(%%rsi), %%rbx\n" 
      "mov  8(%%rsi), %%rsp\n"
      "mov 16(%%rsi), %%rbp\n"
      "mov 24(%%rsi), %%r12\n"
      "mov 32(%%rsi), %%r13\n"
      "mov 40(%%rsi), %%r14\n"
      "mov 48(%%rsi), %%r15\n"
      "mov 56(%%rsi), %%rdi\n"
      "mov 64(%%rsi), %%rsi\n"

      "ret"
      :
      :
      : "memory"
      );
}
