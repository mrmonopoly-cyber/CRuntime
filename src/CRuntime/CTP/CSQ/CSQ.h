#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CSAQueue.h
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
WARNING("using default CSAQ_CAPACITY")
#endif // !defined (CSAQ_CAPACITY)

#define CSQ_CAPACITY (1<<CSQ_CAPACITY_EXP)

/*!< from 0 (highest priority) to N (lowest priority)*/
typedef enum
{
  TaskType_System=0,
  TaskType_User,

  __NUM_TaskType,
}TaskType;

typedef int (*TaskEntry) (void* input);
typedef struct{
  Context ctx;
  Context* caller;
  TaskEntry entry;
  void* arg;
  TaskType type;
}CTask;

typedef struct{
  StackView stack;
  TaskEntry entry;
  void* arg;
}CTaskDescription;

typedef struct CSAtomicQueue{
  CTask* list[CSQ_CAPACITY];
  atomic_size_t write_cursor;
  atomic_size_t read_cursor;
}CSAQ;

typedef struct CSQueue{
  CTask* list[CSQ_CAPACITY];
  atomic_size_t write_cursor;
  atomic_size_t read_cursor;
}CSQ;

typedef CRESULT_TEMPLATE(CTask*, CRStatus) CSAQPopRes;
typedef CRESULT_TEMPLATE(CTask*, CRStatus) CSQPopRes;

#define CTask_init(ENTRY, ARG, TYPE) ((CTask){.entry=(ENTRY), .arg=ARG, .type=TYPE})

/**
 * \brief initialize an already allocated CSAQ
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN CSAQ_init(CSAQ* const self);

/**
 * \brief add a reference of an existing task to CSAQ, by doing so the executor responsible for
 * \brief this particular instance will eventually manage the task
 *
 * @param self pointer to the instance
 * @param task pointer to an already allocated and instantiated task
 *
 * @return see \ref CRReturn in errors.h
 */
CRRETURN CSAQ_push_try(CSAQ* const self, CTask* const task);

/**
 * \brief return a reference to a task, it does not remove the reference from self
 *
 * @param self pointer to the instance
 *
 * @return If Ok the Result contains a valid reference to the task, it cannot be NULL
 * @return If err see \ref CRReturn in errors.h
 *
 */
CRESULT_RETURN(CSAQPopRes) CSAQ_pop_try(CSAQ* const self);

/**
 * \brief return the amount of task references in self
 *
 * @param self pointer to the instance
 *
 * @return see \ref CRReturn in errors.h
 */
size_t CSAQ_size(const CSAQ* const self);











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
