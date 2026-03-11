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

#define CTaskTemplate(T_INPUT, MAX_NUM)                                                           \
  struct{                                                                                         \
    CRStack stack;                                                                                \
    const taskAction entry;                                                                       \
    const size_t input_size;                                                                      \
    T_INPUT input[MAX_NUM];                                                                       \
  }

typedef CTaskTemplate(char, ) CTask;

/**
 * \brief define max amount of task in the queue at the same time
 */
#ifndef TASK_POOL_MAX_CAPACITY
#define TASK_POOL_MAX_CAPACITY 128
WARNING("using default TASK_POOL_MAX_CAPACITY")
#endif // !TASK_POOL_MAX_CAPACITY

typedef struct CTaskPool{
  Context task_pool[TASK_POOL_MAX_CAPACITY];
  char index_bitmap[(TASK_POOL_MAX_CAPACITY/(sizeof(char)*8)) + sizeof(char)];
}CTP;

typedef CRESULT_TEMPLATE(Context*, CRStatus) CTPPopRes;

/**
 * \brief initialize at task pool
 */
CRReturn CTP_init(CTP* const restrict self);

/**
 * \brief add a new specialized task in pool
 * \brief a specialized task can be obtained by defining a CTask type using the template.
 * \brief ser \ref CTaskTemplate
 *
 * @param self pointer to an initialized task pool
 * @param task pointer to a specialized CTask instance
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
#define CTP_add_task(self, task) _CTP_add_task((self), (CTask*) (task));
CRReturn _CTP_add_task(CTP* const restrict self, CTask* task);

/**
 * \brief get a pointer to the next task's context to execute. 
 * \important the task is not removed from the pool
 *
 * @param self pointer to an initialized task pool
 * @return if Ok ptr to task's context
 * @rerurn if Err CRstatus
 */
CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict self);
