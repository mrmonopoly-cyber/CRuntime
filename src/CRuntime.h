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
#include <CResult.h>
#include <CVector.h>

typedef uint8_t CRuntimeStatus;
#define STATUS_OK 0
#define STATUS_FAILED_SAVING_NEW_TASK 1

typedef struct {
  CRuntimeStatus status;
  const char* description;
}CRuntimeError;

typedef TEMPLATE(bool, CRuntimeError) CRuntimeResult;

typedef struct {
  void (*task_fun)(void *);
  void* task_input;
  size_t req_memory;
}CTask;

typedef C_VECTOR_TEMPLATE(CTask) TaskVector;

typedef struct {
  TaskVector task_waiting_queue;
}CRuntime;

typedef struct{
  size_t memory; //INFO: in Bytes
  size_t queue_size;
}CRuntimeInitOpt;
#define CRUNTIME_INIT_DEFAULT 0

#define RUNTIME_RETURN RETURN_TYPE(CRuntimeResult)

RUNTIME_RETURN _CRuntime_init(CRuntime* const self, const CRuntimeInitOpt opt);
#define CRuntime_init(self, ...) _CRuntime_init((self), ((CRuntimeInitOpt) {__VA_ARGS__}))

RUNTIME_RETURN CRuntime_add_task(CRuntime* const self, CTask const task);
