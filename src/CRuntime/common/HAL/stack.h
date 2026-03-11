#pragma once

// Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file stack.h
 * \brief Interface to create and modify an execution stack
 *
 * \author Alberto Damo
 * \date 2026
 */

#include "CResult.h"
#include <stddef.h>

#include <CRuntime/common/errors/errors.h>

typedef struct{
  void* const buf;
  const size_t size;
}CRStack;

typedef struct{
  const void* buf;
  const size_t size;
}CRSDataBuffer;

/**
 * \brief initialize a stack using an already allocated buffer
 * @param STACK address in memory of the buffer
 * @param SIZE size in byte of the allocated buffer
 */
#define CRStackInit(STACK,SIZE) ((CRStack){.buf=(STACK), .size=(SIZE)})

typedef CRESULT_TEMPLATE(void*, CRStatus) CRSPushRes;
/**
 * \brief push some data into an already initialized stack
 * @param input input data to push
 * @return Result ok address to first element in the stack, 
 * @return Result err CRStatus with CR_STATUS_ERR_STACK
 */
CRESULT_RETURN(CRSPushRes) CRStack_push(CRStack* const restrict self, const CRSDataBuffer input);

/**
 * \brief pop some data from an already initialized stack
 * \important if no data is available o_out remain unmodified and the error CR_STATUS_ERR_EMPTY_STACK is returned with a proper description
 * @param o_out output data buffer
 * @return return a CResult type. see \ref CRReturn for more info
 */
CRRETURN CRStack_pop(CRStack* const restrict self, const CRSDataBuffer *o_out);
