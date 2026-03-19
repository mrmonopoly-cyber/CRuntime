#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 *
 * \file CCTX.h
 * \brief object with all the data the needs to be shared among all the component of the runtime
 *
 * \author Alberto Damo
 * \date 2026
 */

#include <CRuntime/common/log/log.h>

typedef struct CContext{
  CRL* logger;
}CCTX;
