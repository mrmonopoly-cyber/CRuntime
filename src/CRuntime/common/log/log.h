#pragma once

//Copyright (c) 2026 Alberto Damo. All Rights Reserved.

/**
 * \file log.h
 * \brief CRruntime lock-free async log system which writes the message in a file.
 * \brief It supports different Log's level.
 *
 *
 * \author Alberto Damo
 * \date 2026
 *
 */

#include <stdatomic.h>
#include <stddef.h>

#include <CRuntime/common/errors/errors.h>

typedef enum{
 Trace=0,
 Debug,
 Info,
 Warning,
 Error,
}CRLogLevel;

typedef struct{
  void* log_file;
  const char* log_file_path;
  atomic_flag lock; //FIXME: breal lock-free requirements
}CRLogger;

typedef struct{
  const char* log_file_path;
  CRLogger* const logger;
}CRLogOpt;

#ifndef DEFAULT_LOG_FILE_DIR_PATH
#define DEFAULT_LOG_FILE_DIR_PATH "."
#endif // !DEFAULT_LOG_FILE_DIR_PATH

#ifndef NO_LOG
CRReturn _CRLog_init(const CRLogOpt opt);
CRRETURN _CRLog(CRLogger* self,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* msg);
#define MY_LOG(LOG, LEV, MSG) _CRLog(LOG, __FILE__, __LINE__, (LEV), (MSG));
#define LOG(LEV, MSG) _CRLog(NULL, __FILE__, __LINE__, (LEV), (MSG))
CRReturn _CRLog_destroy(CRLogger* self);
#else
#define CRLog_init(path)
#define LOG(LEV, MSG)
#endif // !NO_LOG

