#include "log.h"
#include "CResult.h"
#include "CRuntime/common/CVAQ/CVAQ.h"
#include "CRuntime/common/HAL/debug.h"
#include "CRuntime/common/errors/errors.h"

#include <CRuntime/common/HAL/HAL.h>
#include <CRuntime/common/utils/utils.h>
#include <assert.h>

#ifndef NO_LOG

static CRL g_default_logger;

#ifndef MAX_STRING
#define MAX_STRING 4096
#endif /* ifndef MAX_STRING */

#define LOG_FILE_BASE_NAME "/CRLog_"
#define LOG_FILE_NAME_INDEX_SPACE "XXX"
#define LOG_FILE_NAME_EXTRA_PADDING "\0\0\0\0"

CRReturn _CRLog_init(const CRLogOpt opt)
{
  CRL* self = opt.logger ? opt.logger : &g_default_logger;
  CRReturn err ={0};
  const size_t num_queues = sizeof(self->data_to_log)/sizeof(self->data_to_log[0]);
  char default_log_file_path[] =
  {
    DEFAULT_LOG_FILE_DIR_PATH
    LOG_FILE_BASE_NAME
    LOG_FILE_NAME_INDEX_SPACE
    LOG_FILE_NAME_EXTRA_PADDING
  };

  cr_memset(self,0 ,sizeof(*self));

  for(size_t i=0; i<num_queues; i++ )
  {
    TRY(CVAQ_init(&self->data_to_log[i].data_to_log));
  }

  char* cursor =
    default_log_file_path +
    (sizeof(DEFAULT_LOG_FILE_DIR_PATH) -1) +
    (sizeof(LOG_FILE_BASE_NAME) -1);

  if(self->log_file || self->log_file_path)
  {
    err = ERR(CR_STATUS_ERR_INIT, "loggger already initialized");
    goto bad;
  }

  if(opt.log_file_path)
  {
    self->log_file_path = opt.log_file_path;
    CRESULT_ERR_MATCH(CR_open_file(opt.log_file_path),
        err_res,
        {
          err = ERR(err_res.status, err_res.description);  
          goto bad;
        }
    );
  }
  else
  {
    size_t log_index =0;
    char* temp_cursor = cursor;
    CROpenFile res_open = {0};
    const size_t size = 
      sizeof(LOG_FILE_NAME_INDEX_SPACE) +
      sizeof(LOG_FILE_NAME_EXTRA_PADDING)
      -2;

    do
    {
      temp_cursor = cursor;

      //resetting cursor
      cr_memset(temp_cursor, 0, size);

      //appending XXX.log suffix
      temp_cursor += cr_vsnprintf(temp_cursor, sizeof(default_log_file_path), "%d.log", log_index);

      //opening file
      res_open = CR_open_file(default_log_file_path);

      log_index++;
    }
    while(log_index < 999 && CRESULT_IS_ERR(res_open));

    CRESULT_ERR_MATCH(res_open,
        e_err,{
          err = ERR(e_err.status, e_err.description);
          goto bad;
        }
    );

    self->log_file = CRESULT_OK_VAL(res_open);
  }

  return OK();

bad:
  self->log_file_path = NULL;
  self->log_file = NULL;
  return err;
}

CRESULT_RETURN(ResPopQueue) _CRLog_get_queue(CRL* rl, const size_t queue_index)
{
  CRL* self = rl ? rl : &g_default_logger;
  const size_t num_queues = sizeof(self->data_to_log)/sizeof(self->data_to_log[0]);

  assert(queue_index < num_queues);

  return CRESULT_T_OK(ResPopQueue, &self->data_to_log[queue_index]);
}

CRRETURN _CRLog(CRLWorker* self,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* msg)
{
  const size_t next_free = self->bucket_next_free;
  LogInfo* log = &self->bucket[next_free];

  if(log->msg[0] != '\0')
  {
    return ERR(CR_STATUS_ERR_FULL, "worker's log queue is full");
  }

  self->bucket_next_free = (next_free +1) % INPUT_LOG_QUEUE_SIZE;
  char* cursor = log->msg;

  //FIXME: MAX_STRING is wrong
  switch (level)
  {
    case Trace:
      cursor += cr_vsnprintf(cursor, MAX_STRING, "[TRACE]:");
      break;
    case Debug:
      cursor += cr_vsnprintf(cursor, MAX_STRING, "[DEBUG]:");
      break;
    case Info:
      cursor += cr_vsnprintf(cursor, MAX_STRING, "[INFO]:");
      break;
    case Warning:
      cursor += cr_vsnprintf(cursor, MAX_STRING, "[WARNING]:");
      break;
    case Error:
      cursor += cr_vsnprintf(cursor, MAX_STRING, "[ERROR]:");
      break;
  }

  cursor += cr_vsnprintf(cursor, MAX_STRING, "%s.%d: %s\n\r", file, line, msg);


  return CVAQ_push_try(&self->data_to_log, log);
}

CRRETURN _CRLog_drain_x(CRL* rl, const size_t log_per_queue)
{
  CRL* self = rl ? rl : &g_default_logger;
  LogInfo* msg= NULL;
  CLAQ* queue = NULL;
  const size_t num_queues = sizeof(self->data_to_log)/sizeof(self->data_to_log[0]);

  for(size_t i=0; i<num_queues; i++)
  {
    for(size_t j=0; j<log_per_queue; j++)
    {
      queue = &self->data_to_log[i].data_to_log;
      CRESULT_FULL_MATCH(CVAQ_pop_try(queue),
          res_val,
          {
            msg  = res_val;
            CRESULT_ERR_MATCH(CR_write_to_file(self->log_file, msg->msg, cr_strlen(msg->msg)),
                err,
                {
                  UNUSED(err);
                  TODO("log file write error not managed");
                }
            );
          },
          {
            if(res_val.status != CR_STATUS_ERR_EMPTY)
            {
              TODO("log file queue pop error unmanaged");
            }
            break;
          }
      );
    }
  }


  return OK();
}

CRReturn _CRLog_destroy(CRL* rl)
{
  CRL* self = rl ? rl : &g_default_logger;
  return CR_close_file(self->log_file);
}

#endif //!NO_LOG
