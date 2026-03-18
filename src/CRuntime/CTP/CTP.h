#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CTP.h
 * \brief Stores and organizes tasks
 *
 * \author Alberto
 * \date 2026
 */

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/common/common.h>

#include <CRuntime/CScheduler/CScheduler.h>

#include "CSQ/CSQ.h"

#ifdef CTP_CAPACITY
#errors "CTP_CAPACITY is for internal use and cannot be redefined"
#else
#define CTP_CAPACITY (CSQ_CAPACITY * CR_MAX_NUM_OF_CORES)
#endif // CTP_CAPACITY

#ifndef CTP_MAX_INPUT_TASKS
#define CTP_MAX_INPUT_TASKS 8
#endif // !CTP_MAX_INPUT_TASKS

typedef enum{
  SystemTask_collect=0,
  SystemTask_schedule,

  __NUM_SystemTask
}SystemTask;

typedef struct CTaskPool{
  CTaskDescription input_tasks[CTP_MAX_INPUT_TASKS];
  size_t input_tasks_cursor;
  CTask list[CTP_CAPACITY];
  struct __SystemStackInfo{
    CTask task;
    StackView stack;
  }system_tasks[__NUM_SystemTask];
}CTP;

typedef CRESULT_TEMPLATE(CTask*, CRStatus) CTPPopRes;

/**
 * \brief initialize at task pool
 *
 * @param self pointer to a valid uninitialized instance of CTP
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
