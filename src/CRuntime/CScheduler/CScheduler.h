#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CScheduler.h
 * \brief executor for a core
 *
 * \author Alberto Damo
 * \date 2026
 */

#include "CRuntime/CCTX/CCTX.h"
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/common/utils/utils.h>
#include <CRuntime/common/HAL/HAL.h>
#include <CRuntime/common/CVAQ/CVAQ.h>
#include <CRuntime/common/CVQ/CVQ.h>

#if !defined (CSQ_CAPACITY)
#define CSQ_CAPACITY_EXP (5)
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
  const char* name;
  void* arg;
  TaskType type;
}CTask;

#define CTask_init(ENTRY, ARG, NAME, TYPE) ((CTask){.entry=ENTRY, .arg=ARG, .name=NAME, .type=TYPE})

typedef struct{
  StackView stack;
  TaskEntry entry;
  const char* name;
  void* arg;
}CTaskDescription;

typedef CR_QUEUE_TEMPLATE(CTask*, CSQ_CAPACITY) CSQ;
typedef CR_ATOMIC_QUEUE_TEMPLATE(CTask*, CSQ_CAPACITY) CSAQ;

typedef struct CScheduler{
  CSAQ world_task_queue[__NUM_TaskType];
  CSQ local_queue;
  CSAQ drain_queue;
  void* idle_task_stack;
  Context idle_ctx;
  Context ctx;
  Context* active_ctx;
  size_t worker_id;
  CCTX* cr_ctx;
}CS;

/**
 * \brief initialize the memory for the executor
 *
 * @param self pointer to the executor
 * @param worker_id unique id for the worker in range [0 ... CR_MAX_NUM_OF_CORES -1]
 * @param cr_ctx pointer to runtime context
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_init(CS* const self, const size_t worker_id, CCTX* cr_ctx);

/**
 * \brief synchronously run the executor.
 * \important takes control of the thread
 *
 * @param cs pointer to the executor
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_run(CS* const restrict cs);
