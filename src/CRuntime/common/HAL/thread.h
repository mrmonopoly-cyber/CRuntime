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

#include <stdint.h>

#include <CRuntime/common/errors/errors.h>

#ifndef THREAD_MEM_INFO_SIZE
#error "THREAD_MEM_INFO_SIZE not set. To do that add -DTHREAD_MEM_INFO_SIZE=<size> to your compiler input"
#endif // !THREAD_MEM_INFO_SIZE
#ifndef THREAD_MEM_INFO_ALIGNEMENT
#error "THREAD_MEM_INFO_ALIGNEMENT not set. To do that add -DTHREAD_MEM_INFO_ALIGNEMENT=<size> to your compiler input"
#endif // !THREAD_MEM_INFO_ALIGNEMENT

typedef int (*ThreadEntry)(void*);
typedef uintptr_t ThreadId;

typedef struct __attribute__((__aligned__(THREAD_MEM_INFO_ALIGNEMENT))){
  char data[THREAD_MEM_INFO_SIZE];
}ThreadMem;

typedef CRESULT_TEMPLATE(ThreadId, CRStatus) ThreadStart;
typedef CRESULT_TEMPLATE(int, CRStatus) ThreadWait;

typedef struct{
  ThreadEntry entry;
  void* arg;
}ThreadExec;


/**
 * \brief initialize the new memory for a future thread
 *
 * @input self thread memory descriptor
 *
 * @return CRStatus see \ref CRStatus for more info
 */
CRReturn Thread_allocate_memory(ThreadMem* const self);

/**
 * \brief return a pointer to a new stack for a future thread
 *
 * @input stack already allocated stack for the execution
 * @input entry fun ptr with input
 *
 * @return ThreadId if Ok: unique id of the thread
 * @return CRStatus if if Err with a proper description of the failure
 */
CRESULT_RETURN(ThreadStart) Thread_start(ThreadMem* const stack, const ThreadExec entry);

/**
 * \brief waith until the thread ends and return its exit status or an error message if there was one
 *
 * @input self id of the executing thread
 *
 * @return int if Ok: exit status of the thread
 * @return CRStatus if if Err with a proper description of the failure
 */
CRESULT_RETURN(ThreadWait) Thread_wait(ThreadId const self);
