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
}CScheduler;

CRRETURN CScheduler_init(CScheduler* const self, CTask* const task_pool);
