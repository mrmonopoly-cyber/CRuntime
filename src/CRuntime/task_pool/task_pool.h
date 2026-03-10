#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Store and organize tasks
 */

#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/common.h>

#ifndef TASK_POOL_MAX_CAPACITY
#define TASK_POOL_MAX_CAPACITY 128
#endif // !TASK_POOL_MAX_CAPACITY

typedef void (*taskAction)(void*) ;
typedef struct {
  taskAction action;
  void* task_input;
}CTask;

typedef CRESULT_TEMPLATE(CRPid, CRStatus) CTInitRet;

CRESULT_RETURN(CTInitRet) CTask_init(void);
