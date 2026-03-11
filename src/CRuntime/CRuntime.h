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

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/common.h>
#include <CRuntime/CTask/CTask.h>

typedef struct {
  CTP task_pool;
  Context runtime_context;
}CRuntime;

CRRETURN
CRuntime_init(CRuntime* const restrict self, const CRStack stack);

CRRETURN
CRuntime_add_task(CRuntime* const restrict self,
    const taskAction entry, void* input, const CRStack stack);

CRRETURN
CRuntime_start_sync(CRuntime* const restrict self);

CRRETURN
CRuntime_terminate(CRuntime* const restrict self);
