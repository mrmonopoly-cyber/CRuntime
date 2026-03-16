#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CRuntime.h 
 * \brief - executes concurrently, different, independent tasks autonomously
 * \brief - define different tasks
 * \brief - switch between tasks
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/CScheduler/CScheduler.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/common.h>
#include <CRuntime/CTP/CTP.h>

typedef struct {
  CTP task_pool;
  struct{
    StackInfo stack;
    ThreadId id;
  }engines[CR_MAX_NUM_OF_CORES];
}CRuntime;

typedef struct{
  const size_t active_cores; /*!< num of usable cores of CR_MAX_NUM_OF_CORES*/
}CRuntimeInitOpt;

/**
 * \brief default values for runtime's parameters
 */
#define CRUNTIME_DEFAULTS .active_cores = CR_MAX_NUM_OF_CORES,

/**
 * \brief initialize runtime using the stack given by the user.
 * \important it does not start the runtime
 *
 * @param self pointer to the runtime
 * @param opt optional parameters to tune the runtime \ref CRuntimeInitOpt.
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN _CRuntime_init(CRuntime* const restrict self, const CRuntimeInitOpt opt);
#define CRuntime_init(self,...) _CRuntime_init((self), ((CRuntimeInitOpt){__VA_ARGS__}))

/**
 * \brief add a new task to the runtime
 * \important the task does not start immediately, it's scheduled by the runtime
 *
 * @param self the runtime
 * @param action the function executed by the task with its input (optionally)
 * @param input input variable for the entry function
 * @param stack memory where the task will execute
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CRuntime_add_task(
    CRuntime* const restrict self,
    const TaskEntry fun,
    void* arg,
    const StackView stack);

/**
 * \brief synchronously starts the runtime
 * \important it takes control of the current execution thread
 *
 * @param self the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CRuntime_start_sync(CRuntime* const restrict self);

/**
 * \brief stop the runtime if it's executing, otherwise it does nothing
 *
 * @param self the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CRuntime_terminate(CRuntime* const restrict self);

/**
 *
 * \brief interrupt the execution of a task giving back the control to the runtime
 *
 * \important this function can only be used inside of a task, using it outside is Undefined Behaviour
 *
 * @param env environment about the scheduler, it's given to the task by the runtime itself
 */
#define CRuntime_yield() yield_real()
