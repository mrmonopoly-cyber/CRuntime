#pragma once

/*
 * Copyright (c) 2026 Alberto Damo. All Rights Reserved.
 *
 * Error definitions
 */

#include <CResult.h>
#include <stdint.h>

#define CR_STATUS_OK 0
#define CR_STATUS_ERR_PROCESS 1
#define CR_STATUS_ERR_EMPTY_STACK 2
#define CR_STATUS_ERR_STACK 3
#define CR_STATUS_ERR_QTASK_EMPTY 4
#define CR_STATUS_ERR_QTASK_FULL 5
#define CR_STATUS_ERR_UNREACHABLE_CODE 6

#define CR_STATUS_ERR_UNKNOWN 99

typedef struct{
  uint16_t status;
  const char* description;
}CRStatus;

typedef CRESULT_TEMPLATE(bool, CRStatus) CRReturn;

#define CRRETURN CRESULT_RETURN(CRReturn)

#define OK() CRESULT_T_OK(CRReturn, 0)
#define ERR(...) CRESULT_T_ERR(CRReturn, ((CRStatus){__VA_ARGS__}))
#define TRY(res) do{ if(CRESULT_IS_ERR((res))) return res; }while(0)
