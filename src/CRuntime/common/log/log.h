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

#include "CResult.h"
#include <stdatomic.h>
#include <stddef.h>

#include <CRuntime/common/utils/utils.h>
#include <CRuntime/common/CVAQ/CVAQ.h>
#include <CRuntime/common/errors/errors.h>

#ifndef MAX_LOG_MSG_LENGTH
#define MAX_LOG_MSG_LENGTH (256)
#endif // !MAX_LOG_MSG_LENGTH

#ifndef INPUT_LOG_QUEUE_SIZE
#define  INPUT_LOG_QUEUE_SIZE (16)
#endif // !INPUT_LOG_QUEUE_SIZE

#ifndef DEFAULT_LOG_FILE_DIR_PATH
#define DEFAULT_LOG_FILE_DIR_PATH "."
#endif // !DEFAULT_LOG_FILE_DIR_PATH

typedef enum{
 Trace=0,
 Debug,
 Info,
 Warning,
 Error,
}CRLogLevel;

typedef struct{
  char msg[MAX_LOG_MSG_LENGTH];
}LogInfo;


typedef CR_ATOMIC_QUEUE_TEMPLATE(LogInfo*, INPUT_LOG_QUEUE_SIZE) CLAQ;

typedef struct{
  CLAQ data_to_log;
  LogInfo bucket[INPUT_LOG_QUEUE_SIZE];
  size_t bucket_next_free;
}CRLWorker;

typedef struct CRLogger{
  void* log_file;
  const char* log_file_path;
  CRLWorker data_to_log[CR_MAX_NUM_OF_CORES + 1];
}CRL;

typedef struct{
  const char* log_file_path;
  CRL* const logger;
}CRLogOpt;

typedef CRESULT_TEMPLATE(CRLWorker*, CRStatus) ResPopQueue;

#ifndef NO_LOG
CRReturn _CRLog_init(const CRLogOpt opt);

CRESULT_RETURN(ResPopQueue) CRLog_get_queue(CRL* self, const size_t queue_index);

CRRETURN _CRLog(CRLWorker* self,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* msg);
#define LOG(LOG, LEV, MSG) _CRLog(LOG, __FILE__, __LINE__, (LEV), (MSG))

void CRLog_drain_x(CRL* self, const size_t log_per_queue);

CRReturn CRLog_destroy(CRL* self);
#else
#define CRLog_init(path)

#define CRlog_get_queue(queue_index)

#define LOG(LOG, LEV, MSG)

CRReturn _CRLog_destroy()
#endif // !NO_LOG
