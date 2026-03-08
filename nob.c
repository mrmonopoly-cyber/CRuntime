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

static int _build_obj(const char* base_src_dir)
{
  int result=0;
  Cmd cmd = {0};
  Dir_Entry dir_entry = {0};
  char* src_cursor = NULL;
  char src_temp_path[PATH_MAX] = {0};
  int base_src_temp_path_length = 0;

  strncat(src_temp_path, base_src_dir, strlen(base_src_dir));
  src_cursor = src_temp_path + strlen(base_src_dir);
  (*src_cursor++) = '/';
  base_src_temp_path_length = strlen(src_temp_path);

  if(!dir_entry_open(base_src_dir, &dir_entry))
  {
    result =1;
    goto end;
  }

  while (dir_entry_next(&dir_entry)){
    char* temp_cursor = src_cursor;
    const int dir_name_length = strlen(dir_entry.name);
    if (
        !strncmp(dir_entry.name, "..", dir_name_length) || 
        !strncmp(dir_entry.name, ".", dir_name_length) ||
        !strncmp(dir_entry.name + dir_name_length-2, ".h", 2)
        )
    {
      continue;
    }

    snprintf(temp_cursor, strlen(dir_entry.name) + 1, "%s", dir_entry.name);
    nob_log(INFO, "working on %s", dir_entry.name);

    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-std=c99");
    cmd_append(&cmd, "-pedantic");
    cmd_append(&cmd, "-c");
    cmd_append(&cmd, src_temp_path);

    if (!cmd_run(&cmd, .dont_reset = false)){
      result = 3;
      goto clean_dir;
    };
  };

clean_dir:
  dir_entry_close(dir_entry);
clean_cmd: 
  cmd_free(cmd);
end:
  return result;
}

static int _link_obj(const char* const base_src_dir, const char* o_file)
{
  int result=0;
  Dir_Entry dir_entry = {0};
  Cmd cmd = {0};

  cmd_append(&cmd, "cc");
  cmd_append(&cmd, "-Wall");
  cmd_append(&cmd, "-Wextra");

  if(!dir_entry_open(".", &dir_entry)){
    result =1;
    goto clean_cmd;
  }
  while (dir_entry_next(&dir_entry))
  {
    int written = 0;
    const int dir_name_length = strlen(dir_entry.name);

    if (!strncmp(dir_entry.name, "..", 2) || 
        !strncmp(dir_entry.name, ".", 1) ||
        !strncmp(dir_entry.name, o_file, strlen(dir_entry.name)))
    {
      nob_log(INFO, "skipping %s", dir_entry.name);
      continue;
    }

    nob_log(INFO, "working on: %s", dir_entry.name);
    cmd_append(&cmd,dir_entry.name);
  };

  cmd_append(&cmd, "-o");
  cmd_append(&cmd, o_file);

  if (!cmd_run(&cmd, .dont_reset = false)){
    result = 4;
  }

clean_dir:
  dir_entry_close(dir_entry);
clean_cmd:
  cmd_free(cmd);
end:
  return result;
}

int main(int argc, char** argv)
{
  GO_REBUILD_URSELF(argc, argv);
  int err =0;

  char src_dir[PATH_MAX] = {0};

  const char* root = get_current_dir_temp();
  const int pwd_length = strlen(root);

  strncat(src_dir, root, pwd_length);
  src_dir[pwd_length] = '/';
  strncat(src_dir, SRC_DIR, PATH_MAX - sizeof(SRC_DIR));

  if(!mkdir_if_not_exists(BUILD_DIR)) return 1;
  if(!set_current_dir(BUILD_DIR)) return 2;

  nob_log(INFO, "src dir: %s\n", src_dir);
  nob_log(INFO, "build dir: %s/%s\n", root,BUILD_DIR);

  nob_log(INFO, "building objs");
  if((err = _build_obj(src_dir))) return(err);
  nob_log(INFO, "linking objs");
  if((err = _link_obj(src_dir, O_FILE))) return(err);

  if(!set_current_dir(root)) return(2);

  if(!copy_file(BUILD_DIR"/"O_FILE, O_FILE)) return(4);


  return 0;
}
