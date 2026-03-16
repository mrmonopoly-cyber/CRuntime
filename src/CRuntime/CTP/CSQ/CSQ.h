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

typedef int (*TaskEntry) (void* input);
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

/**
 * \brief initialize an already allocated CSQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN CSQ_init(CSQ* const self);

/**
 * \brief add a reference of an existing task to CSQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the task
 *
 * @param self pointer to the instance
 * @param task pointer to an already allocated and instantiated task
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN CSQ_push_try(CSQ* const self, CTask* const task);

/**
 * \brief return a reference to a task, it does not remove the reference from self
 *
 * @param self pointer to the instance
 *
 * @return If Ok the Result contains a valid reference to the task, it cannot be NULL
 * @return If err see \ref CRReturn in errors.h
 *
 */
CRESULT_RETURN(CSQPopRes) CSQ_pop_try(CSQ* const self);

/**
 * \brief return the amount of task references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t CSQ_size(const CSQ* const self);
