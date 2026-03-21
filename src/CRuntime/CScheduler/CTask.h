#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CTask.h
 * 
 * \brief Type definition of a Task
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/common/HAL/context.h>

/*!< from 0 (highest priority) to N (lowest priority)*/
typedef enum
{
  TaskType_System=0,
  TaskType_User,

  __NUM_TaskType,
}TaskType;

typedef int (*TaskEntry) (void* input);
typedef struct{
  Context ctx;
  Context* caller;
  TaskEntry entry;
  const char* name;
  void* arg;
  TaskType type;
}CTask;

typedef struct{
  StackView stack;
  TaskEntry entry;
  const char* name;
  void* arg;
}CTaskDescription;

#define CTask_init(ENTRY, ARG, NAME, TYPE) ((CTask){.entry=ENTRY, .arg=ARG, .name=NAME, .type=TYPE})

