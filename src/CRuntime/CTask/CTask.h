#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Stores and organizes tasks
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

#ifndef TASK_POOL_MAX_CAPACITY
#define TASK_POOL_MAX_CAPACITY 128
WARNING("using default TASK_POOL_MAX_CAPACITY")
#endif // !TASK_POOL_MAX_CAPACITY

typedef struct CTaskPool{
  Context task_pool[TASK_POOL_MAX_CAPACITY];
  char index_bitmap[(TASK_POOL_MAX_CAPACITY/(sizeof(char)*8)) + sizeof(char)];
}CTP;

typedef CRESULT_TEMPLATE(Context, CRStatus) CTPPopRes;

CRReturn CTP_init(CTP* const restrict self);

CRReturn _CTP_add_task(CTP* const restrict self, CTask* task);
#define CTP_add_task(self, task) _CTP_add_task((self), (CTask*) (task));

CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict self);
