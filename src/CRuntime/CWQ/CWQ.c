#include "CWQ.h"

CRRETURN CWQ_init(CWQ* const self)
{
  memset(self, 0, sizeof(&self));
  atomic_init(&self->next_free_cell,0);

  return OK();
}
