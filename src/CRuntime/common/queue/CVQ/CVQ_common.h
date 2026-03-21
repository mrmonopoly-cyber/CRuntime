#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file CVQ.h
 *
 * \brief Single producer Single consumer queue
 * \important IT ONLY STORES POINTERS
 *
 * \author Alberto Damo
 * \date 2026
 */



#include <stddef.h>

#include <CRuntime/common/errors/errors.h>

#define CR_QUEUE_TEMPLATE(TYPE, MAX_SIZE)\
  struct{\
    size_t write_cursor;\
    size_t read_cursor;\
    TYPE* list[MAX_SIZE];\
  }
#define CR_QUEUE_POP_TEMPLATE(TYPE) CRESULT_TEMPLATE(TYPE*, CRStatus) 

#define CR_QUEUE_MAX_SIZE(self) (sizeof((self)->list)/sizeof((self)->list[0]))


