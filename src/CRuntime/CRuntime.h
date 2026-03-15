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
#include <CRuntime/CTask/CTask.h>

#if !defined(CRUNTIME_SIZE) && !defined(CRUNTIME_ALIGN)
#define CRUNTIME_SIZE 14400
#define CRUNTIME_ALIGN 16
#else
#errro "CRUNTIME_SIZE AND CRUNTIME_ALIGN are for internal use and MUST NOT be defined by user"
#endif // !!defined(CRUNTIME_SIZE) && !defined(CRUNTIME_ALIGN)

typedef struct __attribute__((__aligned__(CRUNTIME_ALIGN))){
  char data[CRUNTIME_SIZE];
}CRuntime;

/**
 * \brief initialize runtime using the stack given by the user.
 * \important it does not start the runtime
 *
 * @param self pointer to the runtime
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CRuntime_init(CRuntime* const restrict self);

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
 * @input env environment about the scheduler, it's given to the task by the runtime itself
 */
void CRuntime_yield(void* env);
