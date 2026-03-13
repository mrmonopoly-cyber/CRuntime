#include "CTask.h"

#include <stdint.h>

#include <CResult.h>

#include <CRuntime/common/HAL/context.h>
#include <CRuntime/common/HAL/debug.h>
#include <CRuntime/common/errors/errors.h>


CRReturn CTP_init(CTP* const restrict self)
{
  memset(self, 0, sizeof(*self));
  return OK();
}

CRReturn CTP_add_task(CTP* const restrict self, const CTask task)
{
  const size_t size_index = sizeof(self->index_bitmap[0])*8;

  //TODO: more efficient
  for(uint16_t i=0;i<sizeof(self->task_pool)/sizeof(self->task_pool[0]);i++)
  {
    const uint16_t cell = i/size_index;
    const uint16_t remainder = (1<<(i%size_index));
    if (!(self->index_bitmap[cell] & remainder))
    {
      CTaskEnv* const env = &self->task_env[i];
      const TaskAction action ={
        .entry = task.entry,
        .arg = task.arg,
        .env = env,
      };

      env->runtime_context = task.runtime_ctx;
      env->task_context = &self->task_pool[i];

      TRY(Context_init(env->task_context, task.stack, action));

      self->index_bitmap[cell] |= remainder;
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
      //INFO: task is not removed from the queue
      return CRESULT_T_OK(CTPPopRes, &self->task_pool[i]);
    }
  }
  
  return ERR_CTPPORES(CR_STATUS_ERR_QTASK_EMPTY,"empty task queue, nothing to do");
}
#undef ERR_CTPPORES
