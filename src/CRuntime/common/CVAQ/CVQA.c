#include "CVAQ.h"

#include <assert.h>
#include <stdatomic.h>

#include <CRuntime/common/utils/utils.h>

CRRETURN _CVAQ_init(CVAQ* const self)
{
  assert(self);

  cr_memset(self, 0, sizeof(*self));

  atomic_init(&self->read_cursor, 0);
  atomic_init(&self->write_cursor, 0);


  return OK();
}

CRRETURN _CVAQ_push_try(CVAQ* const self, const size_t size, void* const task)
{
  assert(self);
  assert(task);

  size_t write = atomic_load(&self->write_cursor);
  write = (write+1) & (size-1);

  if (write != atomic_load(&self->read_cursor))
  {
    self->list[write] = task;
    atomic_store(&self->write_cursor, write);
    return OK();
  }

  return ERR(CR_STATUS_ERR_FULL, "CVAQ queue full");
}

CRESULT_RETURN(CVAQPopRes) _CVAQ_pop_try(CVAQ* const self, const size_t size)
{
  assert(self);

  size_t read = atomic_load(&self->read_cursor);
  void* p_task = self->list[read];

  if (read != atomic_load(&self->write_cursor))
  {
    read = (read+1) & (size-1);
    p_task = self->list[read];
    atomic_store(&self->read_cursor, read);
    return CRESULT_T_OK(CVAQPopRes, p_task);
  }

  return CRESULT_T_ERR(CVAQPopRes, ((CRStatus) {CR_STATUS_ERR_EMPTY, "CVAQ queue empty"}));
}

size_t _CVAQ_size(const CVAQ* const self, const size_t size)
{
  assert(self);
  size_t read = atomic_load(&self->read_cursor);
  size_t write = atomic_load(&self->write_cursor);

  if(write < read)
  {
    read -= write;
    return size- read;
  }
  return read ? write - (read - 1) : write - read;
}

