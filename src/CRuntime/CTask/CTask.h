#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CTask.h
 * \brief Stores and organizes tasks
 *
 * \author Alberto
 * \date 2026
 */

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/common/common.h>

/**
 * \brief define max amount of task in the queue at the same time
 */
#ifndef TASK_POOL_MAX_CAPACITY
#define TASK_POOL_MAX_CAPACITY 128
WARNING("using default TASK_POOL_MAX_CAPACITY")
#endif // !TASK_POOL_MAX_CAPACITY

typedef int (*TaskEntry) (void* input, void* env);
typedef struct{
  StackView stack;
  TaskEntry entry;
  void* arg;
}CTaskDescription;

typedef struct{
  Context ctx;
  Context* caller;
  TaskEntry entry;
}CTask;

#if !defined (CTP_SIZE) && !defined (CTP_ALIGN)
#define CTP_SIZE 14352
#define CTP_ALIGN 16
#else
#errro "CTP_SIZE AND CTP_ALIGN are for internal use and MUST NOT be defined by user"
#endif // !defined (CTP_SIZE) && !defined (CTP_ALIGN)

typedef struct CTaskPool{
  char data[CTP_SIZE];
}CTP __attribute__((__aligned__(CTP_ALIGN)));

typedef CRESULT_TEMPLATE(CTask*, CRStatus) CTPPopRes;

/**
 * \brief initialize at task pool
 */
CRRETURN CTP_init(CTP* const restrict self);

/**
 * \brief add a new specialized task in pool
 * \brief a specialized task can be obtained by defining a CTask type using the template.
 * \brief ser \ref CTaskTemplate
 *
 * @param self pointer to an initialized task pool
 * @param task to a CTask instance
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CTP_add_task(CTP* const restrict self, CTaskDescription task);

/**
 * \brief get a pointer to the next task's context to execute. 
 * \important the task is not removed from the pool
 *
 * @param self pointer to an initialized task pool
 * @return if Ok ptr to task's context
 * @rerurn if Err CRstatus
 */
CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict self, Context* const restrict caller);
