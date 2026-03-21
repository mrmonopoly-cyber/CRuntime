#define CVQ_INTERNAL
#include "CVQ.h"

#include <assert.h>

#include <CRuntime/common/utils/utils.h>

CRRETURN _CVQ_init(CVQ* const self)
{
  assert(self);

  cr_memset(self, 0, sizeof(*self));

  return OK();
}

CRRETURN _CVQ_push_try(CVQ* const self, const size_t size, void* const task)
{
  assert(self);
  assert(task);

  size_t write = self->write_cursor;

  write = (write+1) & (size -1);

  if (write != self->read_cursor)
  {
    self->list[write] = task;
    self->write_cursor = write;
    return OK();
  }

  return ERR(CR_STATUS_ERR_FULL, "CVQ queue full");
}

CRESULT_RETURN(CVQPopRes) _CVQ_pop_try(CVQ* const self, const size_t size)
{
  assert(self);

  size_t read = self->read_cursor;
  void* p_task = self->list[read];

  if (read != self->write_cursor)
  {
    read = (read+1) & (size -1);
    p_task = self->list[read];
    self->read_cursor = read;
    return CRESULT_T_OK(CVQPopRes, p_task);
  }

  return CRESULT_T_ERR(CVQPopRes, ((CRStatus) {CR_STATUS_ERR_EMPTY, "CVQ queue empty"}));
}

size_t _CVQ_size(const CVQ* const self, const size_t size)
{
  assert(self);

  size_t read = self->read_cursor;
  size_t write = self->write_cursor;

  if(write < read)
  {
    read -= write;
    return size - read;
  }
  return read ? write - (read - 1) : write - read;
}
