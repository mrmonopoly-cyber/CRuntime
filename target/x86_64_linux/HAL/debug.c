#include <CRuntime/common/HAL/debug.h>
#include <stdio.h>


void
_todo(const char* const file, const int line, const char* masg)
{
  printf("%s.%d:%s\n",file,line,masg);
}
