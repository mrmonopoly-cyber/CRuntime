#include <CRuntime/common/HAL/debug.h>

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


void _todo(const char* const file, const int line, const char* masg)
{
  printf("%s.%d:%s\n",file,line,masg);
}

CRESULT_RETURN(CROpenFile) CR_open_file(const char* path)
{
  int fd = open(path,  O_CREAT | O_RDWR| O_EXCL, S_IRUSR | S_IWUSR| S_IRGRP | S_IWGRP);

  if(fd<0)
  {
    return CRESULT_T_ERR(CROpenFile, ((CRStatus){fd, "posix open failed"}));
  }

  return CRESULT_T_OK(CROpenFile, (void*) (uintptr_t) fd);
}

CRRETURN CR_write_to_file(void* file, const char* msg, const size_t size)
{
  int fd = (int) (uintptr_t) file;
  size_t written;

  written = write(fd, msg, size);

  if(written < size)
  {
    return  ERR(CR_STATUS_ERR_UNKNOWN, "not all bytes has been written");
  }

  return OK();
}
