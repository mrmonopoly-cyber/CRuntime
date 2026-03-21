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
#include <stddef.h>

#include <CResult.h>

#include <CRuntime/common/utils/utils.h>
#include <CRuntime/common/errors/errors.h>

#include "CLAQ.h"


#ifndef DEFAULT_LOG_FILE_DIR_PATH
#define DEFAULT_LOG_FILE_DIR_PATH "."
#endif // !DEFAULT_LOG_FILE_DIR_PATH

#ifndef CR_LOG_FILL_THRESHOLD
#define CR_LOG_FILL_THRESHOLD (INPUT_LOG_QUEUE_SIZE/2)
#endif // !CR_LOG_FILL_THRESHOLD

typedef struct{
  CLAQ data_to_log;
  LogInfo bucket[INPUT_LOG_QUEUE_SIZE];
  size_t bucket_next_free;
}CRLWorker;

typedef struct CRLogger{
  void* log_file;
  const char* log_file_path;
  CRLWorker data_to_log[CR_MAX_NUM_OF_CORES + 1];
  size_t most_load;
}CRL;

typedef struct{
  const char* log_file_path;
  CRL* const logger;
}CRLogOpt;

#ifndef NO_LOG
CRReturn _CRLog_init(const CRLogOpt opt);

CRRETURN _CRLog(CRL* rl,
    const size_t worker_id,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* fmt,
    ...);
#define LOG(LOG, ID, LEV, FMT, ...) \
  _CRLog((LOG), (ID), __FILE__, __LINE__, (LEV), (FMT), ##__VA_ARGS__)

void CRLog_drain_x(CRL* self, const size_t log_per_queue);

size_t CRLog_size(CRL* self);

CRReturn CRLog_destroy(CRL* self);
#else
#define CRLog_init(opt) (Ok())
#define CRLog_drain_x(s, l)
#define LOG(LOG, LEV, MSG) (Ok())
#define CRLog_size(self) (0)
CRReturn _CRLog_destroy(s) (Ok())
#endif // !NO_LOG
