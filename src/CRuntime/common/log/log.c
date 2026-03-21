#include "log.h"

#include <assert.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdarg.h>

#include <CRuntime/common/HAL/HAL.h>

#ifndef NO_LOG

static CRL g_default_logger;
static atomic_flag g_draining;

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
    TRY(CLAQ_init(&self->data_to_log[i].data_to_log));
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

CRRETURN _CRLog(CRL* rl,
    const size_t worker_id,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* fmt,
    ...)
{
  assert(worker_id <= CR_MAX_NUM_OF_CORES + 1);

  CRL* p_rl = rl ? rl : &g_default_logger;
  CRLWorker* self = &p_rl->data_to_log[worker_id];
  const size_t next_free = self->bucket_next_free;
  LogInfo* log = &self->bucket[next_free];
  va_list arg;
  size_t fill_level=CLAQ_size(&self->data_to_log);

  if( fill_level >= INPUT_LOG_QUEUE_SIZE)
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

  cursor += cr_vsnprintf(cursor, MAX_STRING, "%s.%d: ", file, line);
  
  va_start(arg, fmt);
  cursor += cr_vsnprintf_arg(cursor, MAX_STRING, fmt, arg);
  va_end(arg);

  cursor += cr_vsnprintf(cursor, MAX_STRING, "\n\r", NULL);

  TRY(CLAQ_push_try(&self->data_to_log, log));

  fill_level++;

  if(fill_level > p_rl->most_load) p_rl->most_load = fill_level;

  return OK();
}

void CRLog_drain_x(CRL* rl, const size_t log_per_queue)
{
  CRL* self = rl ? rl : &g_default_logger;
  LogInfo* msg= NULL;
  CLAQ* queue = NULL;
  const size_t num_queues = sizeof(self->data_to_log)/sizeof(self->data_to_log[0]);

  if(atomic_flag_test_and_set(&g_draining))
  {
    return;
  }

  for(size_t i=0; i<num_queues; i++)
  {
    for(size_t j=0; j<log_per_queue; j++)
    {
      queue = &self->data_to_log[i].data_to_log;
      CRESULT_FULL_MATCH(CLAQ_pop_try(queue),
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

  atomic_flag_clear(&g_draining);

}

size_t CRLog_size(CRL* rl)
{
  CRL* self = rl ? rl : &g_default_logger;
  return self->most_load;
}

CRReturn CRLog_destroy(CRL* rl)
{
  CRL* self = rl ? rl : &g_default_logger;
  return CR_close_file(self->log_file);
}

#endif //!NO_LOG
