#pragma once

#include <errors/errors.h>
#include <CResult.h>

#include <stddef.h>

typedef void* (*process_fun)(void*);
typedef int32_t CRPid;
typedef CRESULT_TEMPLATE(CRPid, CRStatus) CRSpawnReturn;
typedef struct{
  process_fun proc_fun;
  void* input;
}CRProcess;

CRESULT_RETURN(CRSpawnReturn) create_process(const CRProcess self);
