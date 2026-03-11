#include <CRuntime/common/HAL/stack.h>

#include <stdio.h>

#include <CResult.h>
#include <CRuntime/common/common.h>

CRESULT_RETURN(CRSPushRes) CRStack_push(CRStack* const restrict self, const CRSDataBuffer input)
{
  UNUSED(self);
  UNUSED(input);

  printf("pushing on stack var size: %ld\n", input.size);

  return CRESULT_T_OK(CRSPushRes, NULL); //TODO: todo
}
