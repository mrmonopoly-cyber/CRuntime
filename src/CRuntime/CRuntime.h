#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 *
 * Requirements:
 *
 * - executes concurrently, different, independent tasks autonomously
 * - switch between tasks
 * - define different tasks
 */

#include <stddef.h>
#include <stdint.h>

#include <CResult.h>
#include <CVector.h>

#include <CRuntime/common/common.h>
#include <CRuntime/common/HAL/process.h>

#include "task_pool/task_pool.h"

typedef struct {
  CRPid task_pool_pid;
}CRuntime;

typedef struct{
  size_t memory; //INFO: in Bytes
  size_t queue_size;
}CRuntimeInitOpt;

CRRETURN _CRuntime_init(CRuntime* const self, const CRuntimeInitOpt opt);
#define CRuntime_init(self, ...) _CRuntime_init((self), ((CRuntimeInitOpt) {__VA_ARGS__}))

CRRETURN CRuntime_create_task(CRuntime* const self, const CTask task);

CRRETURN CRuntime_terminate(CRuntime* const self);
