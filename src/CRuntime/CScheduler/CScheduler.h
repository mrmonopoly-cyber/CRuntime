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
  void* arg;
  TaskType type;
}CTask;

#define CTask_init(ENTRY, ARG, TYPE) ((CTask){.entry=ENTRY, .arg=ARG, .type=TYPE})

typedef struct{
  StackView stack;
  TaskEntry entry;
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
  CCTX* cr_ctx;
}CS;

/**
 * \brief initialize the memory for the executor
 *
 * @param cs pointer to the executor
 * @param cr_ctx pointer to runtime context
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_init(CS* const cs, CCTX* cr_ctx);

/**
 * \brief synchronously run the executor.
 * \important takes control of the thread
 *
 * @param cs pointer to the executor
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_run(CS* const restrict cs);
