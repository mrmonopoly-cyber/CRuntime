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

#include <CRuntime/common/errors/errors.h>

#ifndef CR_CONTEXT_SIZE
#error "context size has not been defined, to define it define CR_CONTEXT_SIZE globally using -DCR_CONTEXT_SIZE=[context size]"
#endif // !CR_CONTEXT_SIZE

typedef struct{
  void* const stack_buf;
  const size_t stack_size;
}CRStack;

#define CRStackInit(stack,size) ((CRStack){.stack_buf=stack, .stack_size=size})

typedef int (*taskAction)(void* input);
typedef struct __Context{
  const char data[CR_CONTEXT_SIZE];
}Context;

CRRETURN Context_init(Context* const restrict cs,
    taskAction entry,
    void* input,
    void* stack, const size_t stack_size);

void Context_switch(Context* const restrict old_cs, const Context* const restrict new_cs);
