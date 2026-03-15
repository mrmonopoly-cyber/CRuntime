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
  CSQ* task_pool;
  void* idle_task_stack;
  Context idle_ctx;
  Context ctx;
  Context* active_ctx;
}CS;

/**
 * \brief initialize the memory for the executor
 *
 * @input cs pointer to the executor
 * @input task_pool ptr to an already initialized task pool
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_init(CS* const cs, CSQ* const restrict task_pool);

/**
 * \brief synchronously run the executor.
 * \important takes control of the thread
 *
 * @input cs pointer to the executor
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_run(CS* const restrict cs);
