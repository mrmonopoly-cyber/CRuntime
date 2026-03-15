#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSQueue.h
 *
 * \brief Single producer Single consumer lock-free queue for executor's tasks
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stdatomic.h>
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/common/HAL/context.h>

#if !defined (CSQ_CAPACITY)
#define CSQ_CAPACITY_EXP 5
WARNING("using default CSQ_CAPACITY")
#endif // !defined ()

#define CSQ_CAPACITY (1<<CSQ_CAPACITY_EXP)

typedef int (*TaskEntry) (void* input, void* env);
typedef struct{
  Context ctx;
  Context* caller;
  TaskEntry entry;
}CTask;

typedef struct{
  StackView stack;
  TaskEntry entry;
  void* arg;
}CTaskDescription;

typedef struct CSQueue{
  CTask* list[CSQ_CAPACITY];
  atomic_size_t write_cursor;
  atomic_size_t read_cursor;
}CSQ;

typedef CRESULT_TEMPLATE(CTask*, CRStatus) CSQPopRes;

CRRETURN CSQ_init(CSQ* const self);

CRRETURN CSQ_push_try(CSQ* const self, CTask* const task_descr);

CRESULT_RETURN(CSQPopRes) CSQ_pop_try(CSQ* const self);

size_t CSQ_size(const CSQ* const self);
