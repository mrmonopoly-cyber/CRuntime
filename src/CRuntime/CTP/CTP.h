#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CTP.h
 * \brief Stores and organizes tasks
 *
 * \author Alberto
 * \date 2026
 */

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/errors/errors.h>
#include <CRuntime/common/common.h>

#include "CSQ/CSQ.h"

#if !defined(CTP_CAPACITY)
#define CTP_CAPACITY 64
WARNING("default value for CTP_CAPACITY")
#endif /* !defined(CTP_CAPACITY) */

typedef struct{
  Context* ctx;
  size_t core_num;
}CTPCaller;

typedef struct CTaskPool{
  CSQ exec_queue[CR_MAX_NUM_OF_CORES];
  CTask list[CTP_CAPACITY];
  size_t active_cores;
}CTP;

typedef CRESULT_TEMPLATE(CTask*, CRStatus) CTPPopRes;

/**
 * \brief initialize at task pool
 *
 * @param self pointer to a valid uninitialized instance of CTP
 * @param num_active_cores number of active parallel cores in CR_MAX_NUM_OF_CORES
 */
CRRETURN CTP_init(CTP* const restrict self, const size_t num_active_cores);

/**
 * \brief add a new specialized task in pool
 * \brief a specialized task can be obtained by defining a CTask type using the template.
 * \brief ser \ref CTaskTemplate
 *
 * @param self pointer to an initialized task pool
 * @param task to a CTask instance
 *
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CTP_add_task(CTP* const restrict self, CTaskDescription task);
