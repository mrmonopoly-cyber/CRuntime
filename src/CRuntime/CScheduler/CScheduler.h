#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CScheduler.h
 * \brief executor for a core
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>

#if !defined (CS_SIZE) && !defined (CS_ALIGN)
#define CS_SIZE 224
#define CS_ALIGN 16
#else
#errro "CS_SIZE AND CS_ALIGN are for internal use and MUST NOT be defined by user"
#endif // !defined (CS_SIZE) && !defined (CS_ALIGN)

typedef struct __attribute__((__aligned__(CS_ALIGN))) CScheduler{
  char data[CS_SIZE];
}CS;

/**
 * \brief initialize the memory for the executor
 *
 * @input cs pointer to the executor
 * @input task_pool ptr to an already initialized task pool
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_init(CS* const cs, CTP* const restrict task_pool);

/**
 * \brief synchronously run the executor.
 * \important takes control of the thread
 *
 * @input cs pointer to the executor
 *
 * @return look \ref CRStatus for more info
 */
CRRETURN CS_run(CS* const restrict cs);
