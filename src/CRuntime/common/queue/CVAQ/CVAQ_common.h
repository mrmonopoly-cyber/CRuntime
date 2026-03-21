#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CVAQueue.h
 *
 * \brief Single producer Single consumer lock-free queue
 * \important IT ONLY STORES POINTERS
 *
 * \author Alberto Damo
 * \date 2026
 */


#include <stdatomic.h>
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/errors/errors.h>

#define CR_ATOMIC_QUEUE_TEMPLATE(TYPE, MAX_SIZE)\
  struct{\
    atomic_size_t write_cursor;\
    atomic_size_t read_cursor;\
    TYPE* list[MAX_SIZE];\
  }

#define CR_QUEUE_POP_TEMPLATE(TYPE) CRESULT_TEMPLATE(TYPE*, CRStatus) 

#define CR_ATOMIC_QUEUE_MAX_SIZE(self) (sizeof((self)->list)/sizeof((self)->list[0]))
