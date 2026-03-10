#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Store and organize tasks
 */

#include <stddef.h>


#include <CVector.h>
#include <CResult.h>


#include <CRuntime/common/errors/errors.h>

typedef void (*taskAction)(void*) ;

typedef struct {
  taskAction action;
  void* task_input;
}CTask;

typedef C_VECTOR_TEMPLATE(CTask) CTaskVector;
typedef CRESULT_TEMPLATE(CTask, CRStatus) CTaskRet;

typedef struct{
  CTaskVector task_pool;
}CTaskPool;


CRRETURN CTask_push(CTaskPool* const self, const CTask task);

CRESULT_RETURN(CTaskVector) CTask_pop(CTaskPool* const self);
