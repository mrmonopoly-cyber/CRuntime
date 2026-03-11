#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CRuntime.h 
 * \brief executes concurrently, different, independent tasks autonomously
 * \brief define different tasks
 * \brief switch between tasks
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/common.h>
#include <CRuntime/CTask/CTask.h>

typedef struct {
  CTP task_pool;
  Context runtime_context;
}CRuntime;

/**
 * \brief initialize runtime using the stack given by the user.
 * \important it does not start the runtime
 *
 * @param self pointer to the runtime
 * @param stack object defining the stack for the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN
CRuntime_init(CRuntime* const restrict self, const CRStack stack);

/**
 * \brief add a new task to the runtime
 * \important the task does not start immediately, it's scheduled by the runtime
 *
 * @param self the runtime
 * @param entry the function executed by the task
 * @param input input variable for the entry function
 * @param stack memory where the task will execute
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN
CRuntime_add_task(CRuntime* const restrict self,
    const taskAction entry, void* input, const CRStack stack);

/**
 * \brief synchronously starts the runtime
 * \important it takes control of the current execution thread
 *
 * @param self the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN
CRuntime_start_sync(CRuntime* const restrict self);

/**
 * \brief stop the runtime if it's executing, otherwise it does nothing
 *
 * @param self the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN
CRuntime_terminate(CRuntime* const restrict self);
