#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Fetch a task from the task pool and executes it until the task ends or it receive a signal
 */

#include <errors/errors.h>

#include <task_pool/task_pool.h>

typedef struct{
  CTask* task_pool;
}CRScheduler;

CRRETURN CScheduler_init(CRScheduler* const self, CTask* const task_pool);
