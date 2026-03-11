#include "CTask.h"

#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/stack.h>
#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>


CRReturn CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return OK();
}

CRReturn _CTP_add_task(CTP* const restrict self, CTask* task)
{
  Context context;
  void* input_stack_addr = NULL;
  const size_t size_index = sizeof(self->index_bitmap[0])*8;

  CRESULT_FULL_MATCH(
      CRStack_push(&task->stack, (CRSDataBuffer){.buf = task->input, .size=task->input_size}),
        res, {
          input_stack_addr = res;
        },{
          return FROM(res);
        }
  );

  TRY(Context_init(&context, task->entry, input_stack_addr, task->stack));

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    const uint16_t cell = i/size_index;
    const uint16_t remainder = (1<<(i%size_index));
    if (!(self->index_bitmap[cell] & remainder))
    {
      self->index_bitmap[cell] |= remainder;
      self->task_pool[i] = context;
      return OK();
    }
  }

  return ERR(CR_STATUS_ERR_QTASK_FULL,"full task queue, unable to save the task");

}

#define ERR_CTPPORES(...)CRESULT_T_ERR(CTPPopRes, ((CRStatus){__VA_ARGS__}))
CRESULT_RETURN(CTPPopRes) CTP_next(CTP* const restrict self)
{
  const size_t size_index = sizeof(self->index_bitmap[0])*8;

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    const uint16_t cell = i/size_index;
    const uint16_t remainder = (1<<(i%size_index));

    if ((self->index_bitmap[cell] & remainder))
    {
      self->index_bitmap[cell] ^= remainder;
      return CRESULT_T_OK(CTPPopRes, &self->task_pool[i]);
    }
  }
  
  return ERR_CTPPORES(CR_STATUS_ERR_QTASK_EMPTY,"empty task queue, nothing to do");
}
#undef ERR_CTPPORES
