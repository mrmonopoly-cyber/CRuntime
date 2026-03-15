#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file thread.h
 * \brief interface for the management of parallel threads.
 * \important each process needs its own stack but shares the data with the other threads
 *
 * \author Alberto Damo
 * \date 2026
 */

#include "CResult.h"

#include <stddef.h>
#include <stdint.h>

#include <CRuntime/common/errors/errors.h>

typedef int (*ThreadEntry)(void*);
typedef uintptr_t ThreadId;

typedef struct{
  void* low_addr;
  size_t size;
}StackInfo;

typedef CRESULT_TEMPLATE(StackInfo, CRStatus) ThreadAM;
typedef CRESULT_TEMPLATE(ThreadId, CRStatus) ThreadStart;
typedef CRESULT_TEMPLATE(int, CRStatus) ThreadWait;

typedef struct{
  ThreadEntry entry;
  void* arg;
}ThreadExec;


/**
 * \brief allocate a new stack for a future thread
 *
 * @return CRStatus see \ref CRStatus for more info
 */
CRESULT_RETURN(ThreadAM) Thread_allocate_memory(void);

/**
 * \brief return a pointer to a new stack for a future thread
 *
 * @input ptr to an already allocated stack for the execution
 * @input entry fun ptr with input
 *
 * @return ThreadId if Ok: unique id of the thread
 * @return CRStatus if if Err with a proper description of the failure
 */
CRESULT_RETURN(ThreadStart) Thread_start(StackInfo* const stack, const ThreadExec entry);

/**
 * \brief waith until the thread ends and return its exit status or an error message if there was one
 * \brief this function is also responsible to clear the memory of the stack
 *
 * @input self id of the executing thread
 *
 * @return int if Ok: exit status of the thread
 * @return CRStatus if if Err with a proper description of the failure
 */
CRESULT_RETURN(ThreadWait) Thread_wait(ThreadId const self);
