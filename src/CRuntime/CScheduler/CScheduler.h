#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CScheduler.h
 * \brief executor for a core
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/CCTX/CCTX.h>
#include <CRuntime/common/common.h>

#include "CTask.h"
#include "CSAQ.h"
#include "CSQ.h"

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
