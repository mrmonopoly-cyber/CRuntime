#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file context.h
 * \brief Interface to manage context switches
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>

#ifndef CR_CONTEXT_SIZE
#error "context size has not been defined, to define it define CR_CONTEXT_SIZE globally using -DCR_CONTEXT_SIZE=[context size]"
#endif // !CR_CONTEXT_SIZE

#ifndef CR_CONTEXT_ALIGNEMENT
#error "context alignement has not been defined, to define it define CR_CONTEXT_ALIGNEMENT globally using -DCR_CONTEXT_ALIGNEMENT=[context size]"
#endif // !CR_CONTEXT_ALIGNEMENT

typedef int (*entry)(void* input);
typedef struct{
  entry entry;
  void* arg;
}TaskAction;

#define INIT_TASK_ACTION(entry, arg) ((TaskAction){entry, arg})

typedef struct{
  void* start_addr;
  size_t size;
}StackView;

#define INIT_STACK_VIEW(addr, size) ((StackView){addr,size})
#define INIT_STATIC_STACK(addr) ((StackView){(addr),sizeof((addr))})

typedef struct{
  const void* buf;
  const size_t size;
}CRSDataBuffer;

typedef struct __Context{
  char data[CR_CONTEXT_SIZE];
}__attribute__((aligned(CR_CONTEXT_ALIGNEMENT))) Context;
// }ALIGNED_(CR_CONTEXT_ALIGNEMENT) Context; //FIX: why the fucking macro causes compilation error????

CRRETURN Context_init(Context* const restrict cs,
    const StackView stack,
    const TaskAction action);

void Context_switch(Context* const restrict old_cs, const Context* const restrict new_cs);
