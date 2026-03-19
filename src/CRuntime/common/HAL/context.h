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

#ifndef CR_CONTEXT_ALIGNEMENT
#error "context alignement has not been defined, to define it define CR_CONTEXT_ALIGNEMENT globally using -DCR_CONTEXT_ALIGNEMENT=[context alignement]"
#endif // !CR_CONTEXT_ALIGNEMENT

typedef int (*ContextEntry)(void* input);
typedef struct{
  ContextEntry entry;
  void* arg;
}ContextAction;

#define INIT_CONTEXT_ACTION(entry, arg) ((ContextAction){(entry), (arg)})

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

typedef struct{
  char data[CR_CONTEXT_SIZE];
}RegistersState;

typedef struct __Context{
  RegistersState __state;
  StackView __stack;
  ContextAction __action;
}__attribute__((aligned(CR_CONTEXT_ALIGNEMENT))) Context;
// }ALIGNED_(CR_CONTEXT_ALIGNEMENT) Context; //FIX: why the fucking macro causes compilation error????

/**
 * \brief initialize a new Context using the given stack and setting the first function to call
 *
 * @param cs pointer to the context
 * @param stack info about the stack to use
 * @param action info about the function to call with its arguments 
 */
CRRETURN Context_init(Context* const restrict cs,
    const StackView stack,
    const ContextAction action);

/**
 * \brief NAKED function responsible for the context switch.
 * \brief IT saves the current context in old_cs and the load the one from new_cs
 *
 * @param old_cs context object where save the current execution status
 * @param new_cs context object from which load the new context
 */
__attribute__((__naked__))
void Context_switch(Context* const restrict old_cs, const Context* const restrict new_cs);

/**
 * \brief get the address of the env of the current executing thread
 *
 * \important this function is called only inside of a task
 *
 */
__attribute__((__naked__))
void* load_env(void);
