#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CScheduler.h
 * \brief coroutines manager for one core
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/CTask/CTask.h>
#include <CRuntime/common/common.h>

typedef struct CScheduler{
  CTP* task_pool;
  Context ctx;
  Context* active_ctx;
}CS;

void CS_init(CTP* const restrict task_pool);
