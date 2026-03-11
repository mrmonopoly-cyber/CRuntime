#include "CTask.h"

#include <CRuntime/common/errors/errors.h>
#include <string.h>


CRReturn CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return OK();
}
