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

#define CRESULT_NO_PREFIX
#include "CResult.h"

typedef uint8_t CRuntimeStatus;
typedef uint32_t TaskId;
#define STATUS_OK 0

typedef struct {
  CRuntimeStatus status;
  const char* description;
}CRuntimeError;

typedef TEMPLATE(bool, CRuntimeError) CRuntimeResult;

typedef struct {
  void (*task_fun)(void *);
  void* task_input;
}CTask;

typedef struct {
  TaskId master_scheduler;
}CRuntime;

RETURN_TYPE(CRuntimeResult) CRuntime_init(CRuntime* const self);
