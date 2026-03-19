#include "CSQ.h"

#include <assert.h>
#include <stdatomic.h>

CRRETURN CSQ_init(CSQ* const self)
{
  assert(self);

  cr_memset(self, 0, sizeof(*self));

  atomic_init(&self->read_cursor, 0);
  atomic_init(&self->write_cursor, 0);

  return OK();
}

CRRETURN CSQ_push_try(CSQ* const self, CTask* const task)
{
  assert(self);
  assert(task);

  size_t write = atomic_load(&self->write_cursor);
  write = (write+1) & (CSQ_CAPACITY-1);

  if (write != atomic_load(&self->read_cursor))
  {
    self->list[write] = task;
    atomic_store(&self->write_cursor, write);
    return OK();
  }

  return ERR(CR_STATUS_ERR_FULL, "CSQ queue full");
}

CRESULT_RETURN(CSQPopRes) CSQ_pop_try(CSQ* const self)
{
  assert(self);

  size_t read = atomic_load(&self->read_cursor);
  CTask* p_task = self->list[read];

  if (read != atomic_load(&self->write_cursor))
  {
    read = (read+1) & (CSQ_CAPACITY-1);
    p_task = self->list[read];
    atomic_store(&self->read_cursor, read);
    return CRESULT_T_OK(CSQPopRes, p_task);
  }

  return CRESULT_T_ERR(CSQPopRes, ((CRStatus) {CR_STATUS_ERR_EMPTY, "CSQ queue empty"}));
}

size_t CSQ_size(const CSQ* const self)
{
  assert(self);
  size_t read = atomic_load(&self->read_cursor);
  size_t write = atomic_load(&self->write_cursor);

  if(write < read)
  {
    read -= write;
    return CSQ_CAPACITY - read;
  }
  return read ? write - (read - 1) : write - read;
}
