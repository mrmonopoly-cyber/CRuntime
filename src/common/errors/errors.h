#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Error definitions
 */

#include <CResult.h>
#include <stdint.h>

#define CR_STATUS_OK 0
#define CR_STATUS_ERR_PROCESS_SPAWN 1

typedef struct{
  uint16_t status;
  const char* description;
}CRStatus;

typedef CRESULT_TEMPLATE(bool, CRStatus) CRReturn;

#define CRRETURN CRESULT_RETURN(CRReturn)

#define OK() CRESULT_T_OK(CRReturn, 0)
#define ERR(...) CRESULT_T_ERR(CRReturn, ((CRStatus){__VA_ARGS__}))
