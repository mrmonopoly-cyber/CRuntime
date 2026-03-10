#include <fcntl.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR "build"
#define SRC_DIR "src"
#define O_FILE "main"

const char* root = NULL;

static bool _compile_files(Walk_Entry entry)
{
  bool result = true;
  Cmd cmd ={0};

  if (entry.type == NOB_FILE_REGULAR && strncmp(entry.path + strlen(entry.path)-2, ".h", 2))
  {
    nob_log(INFO, "compiling: working on %s",entry.path);

    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-std=c99");
    cmd_append(&cmd, "-pedantic");
    cmd_append(&cmd, "-I../lib/CResult");
    cmd_append(&cmd, "-I../lib/CVector");
    cmd_append(&cmd, "-I../src");
    cmd_append(&cmd, "-c");
    cmd_append(&cmd, entry.path);

    if (!cmd_run(&cmd, .dont_reset = false)) nob_return_defer(false);
    result = true;
  }
  else
  {
    nob_log(INFO, "skipping %s",entry.path);
  }

defer:
  cmd_free(cmd);
  return result;
}

typedef struct{
  Cmd* p_cmd;
  char* buffer;
  int size;
  size_t available_space;
  const char* o_file;
}FindObjFileInput;

static bool _find_obj_file(Walk_Entry entry)
{
  bool result = true;
  Nob_String_View sv = sv_from_cstr(entry.path);
  FindObjFileInput* const input =  (FindObjFileInput*) entry.data;
  const size_t space_needed = sv.count + 2;

  if (entry.type == NOB_FILE_REGULAR && 
      !strncmp(entry.path + sv.count - 2, ".o", 2))
  {
    if (input->available_space < space_needed){
      input->buffer = (char*) NOB_REALLOC(input->buffer, input->size + space_needed);
      memset(input->buffer + input->size, 0, space_needed);
      if (!input->buffer){
        nob_log(ERROR, "realloc failed, by more ram");
        return false;
      }
      input->size += space_needed;
      input->available_space = space_needed;
    }
    strncat(input->buffer, sv.data, input->available_space);
    strncat(input->buffer, " ", input->available_space);
    input->available_space-=(space_needed);
  }

  return result;
}


static int _link_obj(const char* build_dir, const char* o_file)
{
  int result=0;
  Cmd cmd = {0};
  FindObjFileInput input ={
    .p_cmd = &cmd,
    .buffer = NULL,
    .size = 0,
    .available_space =0,
    .o_file = o_file,
  };

  cmd_append(&cmd, "cc");
  cmd_append(&cmd, "-Wall");
  cmd_append(&cmd, "-Wextra");

  if(!walk_dir(build_dir, _find_obj_file, .data = &input)) return_defer(1);

  for(char* cursor=strtok(input.buffer, " ");cursor;cursor = strtok(NULL, " "))
  {
    nob_log(INFO, "file to link: %s", cursor);
    cmd_append(&cmd, cursor);
  }

  cmd_append(&cmd, "-o");
  cmd_append(&cmd, o_file);

  if (!cmd_run(&cmd,0)) return_defer(2);

defer:
  if(input.buffer){
    free(input.buffer);
    input.buffer = NULL;
  }
  cmd_free(cmd);
  return result;
}

int main(int argc, char** argv)
{
  GO_REBUILD_URSELF(argc, argv);
  int err =0;

  char src_dir[PATH_MAX] = {0};
  char build_dir[PATH_MAX] = {0};

  root = get_current_dir_temp();
  const int pwd_length = strlen(root);

  strncat(src_dir, root, pwd_length);
  src_dir[pwd_length] = '/';
  strncat(src_dir, SRC_DIR, PATH_MAX - sizeof(SRC_DIR));

  strncat(build_dir, root, pwd_length);
  build_dir[pwd_length] = '/';
  strncat(build_dir, BUILD_DIR, PATH_MAX - sizeof(BUILD_DIR));

  if(!mkdir_if_not_exists(BUILD_DIR)) return 1;
  if(!set_current_dir(BUILD_DIR)) return 2;

  nob_log(INFO, "src dir: %s", src_dir);
  nob_log(INFO, "build dir: %s", build_dir);

  nob_log(INFO, "building objs");
  if(!walk_dir(src_dir,_compile_files,0)) return err;;

  nob_log(INFO, "linking objs");
  if((err = _link_obj(build_dir, O_FILE))) return(err);

  if(!set_current_dir(root)) return(2);

  if(!copy_file(BUILD_DIR"/"O_FILE, O_FILE)) return(4);


  return 0;
}
