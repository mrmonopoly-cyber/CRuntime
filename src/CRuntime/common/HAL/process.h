#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Interface which defines how to manager new child processes.
 * This is part of the HAL and must be implemented for every target you want to support.
 */

#include <CRuntime/common/errors/errors.h>
#include <CResult.h>

#include <stddef.h>

typedef int (*process_fun)(void*);

typedef int32_t CRPid;
typedef CRESULT_TEMPLATE(CRPid, CRStatus) CRSpawnReturn;
typedef struct{
  process_fun proc_fun;
  void* input;
}CRProcess;

CRESULT_RETURN(CRSpawnReturn) CRProcess_spawn(const CRProcess self);

CRRETURN CRProcess_terminate(const CRPid pid);

void CRProcess_exit(const int exit_val, const char* msg);
