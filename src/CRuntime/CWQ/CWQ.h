#pragma once

/**
 * \file CWQ.h
 *
 * \brief N writer 1 reader lock-free queue for the tasks
 *
 * \author Alberto Damo
 * \date 2026
 */

#include "CRuntime/common/errors/errors.h"
#include <stdatomic.h>

#include <CRuntime/CTP/CSQ/CSQ.h>

#ifdef CTP_CAPACITY
#errors "CTP_CAPACITY is for internal use and cannot be redefined"
#else
#define CTP_CAPACITY (CSQ_CAPACITY * CR_MAX_NUM_OF_CORES)
#endif // CTP_CAPACITY

typedef struct CWaitingQueue
{
  atomic_size_t next_free_cell;
  CTask list[CTP_CAPACITY];
}CWQ;


CRRETURN CWQ_init(CWQ* const self);
