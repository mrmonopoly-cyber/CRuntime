#include "log.h"

#include <CRuntime/common/HAL/HAL.h>

#ifndef NO_LOG

static CRLogger g_default_logger;

#ifndef MAX_STRING
#define MAX_STRING 4096
#endif /* ifndef MAX_STRING */

#define LOG_FILE_BASE_NAME "/CRLog"
#define LOG_FILE_NAME_INDEX_SPACE "XXX"
#define LOG_FILE_NAME_EXTRA_PADDING "\0\0\0\0"

CRReturn _CRLog_init(const CRLogOpt opt)
{
  CRLogger* self = opt.logger ? opt.logger : &g_default_logger;
  CRReturn err ={0};
  char default_log_file_path[] =
  {
    DEFAULT_LOG_FILE_DIR_PATH
    LOG_FILE_BASE_NAME
    LOG_FILE_NAME_INDEX_SPACE
    LOG_FILE_NAME_EXTRA_PADDING
  };

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
      temp_cursor += cr_vsnprintf(temp_cursor, sizeof(default_log_file_path), "%ld.log", log_index);

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

  atomic_flag_clear(&self->lock);

  return OK();

bad:
  self->log_file_path = NULL;
  self->log_file = NULL;
  return err;
}

CRRETURN _CRLog(CRLogger* self,
    const char* file,
    const size_t line,
    const CRLogLevel level,
    const char* msg)
{
  CRLogger* p_self = self;
  char full_msg[MAX_STRING] = {0};
  char* cursor = full_msg;

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

  if(!self)
  {
    p_self = &g_default_logger;
  }

  while(atomic_flag_test_and_set(&p_self->lock));
  CRESULT_ERR_MATCH(CR_write_to_file(p_self->log_file, full_msg, cr_strlen(full_msg)),
      err,{
        atomic_flag_clear(&p_self->lock);
        return ERR(err.status, err.description);
      }
  );
  atomic_flag_clear(&p_self->lock);

  return OK();
}

CRReturn _CRLog_destroy(CRLogger* self);

#endif //!NO_LOG
