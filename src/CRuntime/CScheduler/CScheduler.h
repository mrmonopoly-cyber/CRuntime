#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CScheduler.h
 * \brief executor for a core
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/CTP/CSQ/CSQ.h>
#include <CRuntime/common/common.h>

typedef struct CScheduler{
  CSQ world_task_queue[__NUM_TaskType];
  CSQ local_queue;
  CSQ drain_queue;
  void* idle_task_stack;
  Context idle_ctx;
  Context ctx;
  Context* active_ctx;
}CS;

/**
 * \brief initialize the memory for the executor
 *
 * @param cs pointer to the executor
 * @param task_pool ptr to an already initialized task pool
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_init(CS* const cs);

/**
 * \brief synchronously run the executor.
 * \important takes control of the thread
 *
 * @param cs pointer to the executor
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_run(CS* const restrict cs);
