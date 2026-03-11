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
#define LIB_DIR "lib"
#define ROOT_SRC_DIR "../.."
#define O_FILE "main"

const char* root = NULL;

bool nob_verbose = false;
bool comp_verbose = false;

#define NOB_VERBOSE if(nob_verbose)
#define COMP_VERBOSE if(comp_verbose)

static bool _compile_files(Walk_Entry entry)
{
  bool result = true;
  Cmd cmd ={0};

  if (entry.type == NOB_FILE_REGULAR && strncmp(entry.path + strlen(entry.path)-2, ".h", 2))
  {
    NOB_VERBOSE nob_log(INFO, "compiling: working on %s",entry.path);

    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-std=c11");
    cmd_append(&cmd, "-pedantic");
    cmd_append(&cmd, "-I../"ROOT_SRC_DIR"/"SRC_DIR);
    cmd_append(&cmd, "-I../"ROOT_SRC_DIR"/"LIB_DIR"/CResult");
    cmd_append(&cmd, "-DCR_CONTEXT_SIZE=128");
    COMP_VERBOSE cmd_append(&cmd, "-DVERBOSE");
    cmd_append(&cmd, "-c");
    cmd_append(&cmd, entry.path);

    if (!cmd_run(&cmd, .dont_reset = false)) return_defer(false);
    result = true;
  }
  else
  {
    NOB_VERBOSE nob_log(INFO, "skipping %s",entry.path);
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
    NOB_VERBOSE nob_log(INFO, "file to link: %s", cursor);
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

static void _parse_input(int argc, char** argv)
{
#define CHECK_ARG(input, arg) !strncmp(input[i], (arg), strlen((input[i])))

  for (int i=0; i<argc; i++) {
    if (CHECK_ARG(argv, "-vn") || CHECK_ARG(argv, "--nob_verbose")) {
      nob_verbose = true;
    }
    if (CHECK_ARG(argv, "-vc") || CHECK_ARG(argv, "--comp_verbose")) {
      comp_verbose= true;
    }
    if (CHECK_ARG(argv, "-h") || CHECK_ARG(argv, "--help")) {
      printf("usage ./nob <options>\n");
      printf("\t\t-vn, --nob_verbose\tnob verbose log\n");
      printf("\t\t-vc, --comp_verbose\tcompilation verbose log\n");
      printf("\t\t-h, --help\t\tprint this help\n");
      exit(0);
    }
  }

#undef CHECK_ARG
}

int main(int argc, char** argv)
{
  GO_REBUILD_URSELF(argc, argv);
  int err =0;

  _parse_input(argc, argv);

  char src_dir[PATH_MAX] = {0};
  char root_src_dir[PATH_MAX] = {0};
  char lib_dir[PATH_MAX] = {0};
  char build_dir[PATH_MAX] = {0};

  root = get_current_dir_temp();

  snprintf(src_dir, PATH_MAX, "%s/%s", root, SRC_DIR);
  snprintf(build_dir, PATH_MAX, "%s/%s", root, BUILD_DIR);
  snprintf(root_src_dir, PATH_MAX, "%s/%s/%s", root, ROOT_SRC_DIR, SRC_DIR);
  snprintf(lib_dir, PATH_MAX, "%s/%s/%s", root, ROOT_SRC_DIR, LIB_DIR);

  if(!mkdir_if_not_exists(BUILD_DIR)) return 1;
  if(!set_current_dir(BUILD_DIR)) return 2;

  NOB_VERBOSE nob_log(INFO, "src dir: %s", src_dir);
  NOB_VERBOSE nob_log(INFO, "lib dir: %s", lib_dir);
  NOB_VERBOSE nob_log(INFO, "root_src dir: %s", root_src_dir);
  NOB_VERBOSE nob_log(INFO, "build dir: %s", build_dir);

  NOB_VERBOSE nob_log(INFO, "building root_src objs");
  if(!walk_dir(root_src_dir,_compile_files,0)) return err;;

  NOB_VERBOSE nob_log(INFO, "building lib objs");
  if(!walk_dir(lib_dir,_compile_files,0)) return err;;

  NOB_VERBOSE nob_log(INFO, "building src objs");
  if(!walk_dir(src_dir,_compile_files,0)) return err;;

  NOB_VERBOSE nob_log(INFO, "linking objs");
  if((err = _link_obj(build_dir, O_FILE))) return(err);

  if(!set_current_dir(root)) return(2);

  if(!copy_file(BUILD_DIR"/"O_FILE, O_FILE)) return(4);


  return 0;
}
