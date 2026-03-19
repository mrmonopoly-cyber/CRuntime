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
#define OUT_DIR "out"
#define LIB_DIR "lib"
#define HAL_DIR "HAL"
#define ROOT_SRC_DIR "../.."
#define O_FILE "CR"

const char* root = NULL;

bool nob_verbose = false;
bool comp_verbose = false;
bool comp_debug_mode = false;
bool fpic = false;

#define NOB_VERBOSE if(nob_verbose)
#define COMP_VERBOSE if(comp_verbose)
#define DEBUG_MODE if(comp_debug_mode)

typedef struct{
  Cmd* p_cmd;
  char* buffer;
  int size;
  size_t available_space;
}FindObjFileInput;

static bool _find_src_file(Walk_Entry entry)
{
  bool result = true;
  Nob_String_View sv = sv_from_cstr(entry.path);
  FindObjFileInput* const input =  (FindObjFileInput*) entry.data;
  const size_t space_needed = sv.count + 2;

  if (entry.type == NOB_FILE_REGULAR && !strncmp(entry.path + sv.count - 2, ".c", 2))
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

static bool _find_obj_file(Walk_Entry entry)
{
  bool result = true;
  Nob_String_View sv = sv_from_cstr(entry.path);
  FindObjFileInput* const input =  (FindObjFileInput*) entry.data;
  const size_t space_needed = sv.count + 2;

  if (entry.type == NOB_FILE_REGULAR && !strncmp(entry.path + sv.count - 2, ".o", 2))
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

static int _rm_src(const char* build_dir)
{
  int result = 0;
  Cmd cmd = {0};
  FindObjFileInput input ={
    .p_cmd = &cmd,
    .buffer = NULL,
    .size = 0,
    .available_space =0,
  };

  cmd_append(&cmd, "rm");
  cmd_append(&cmd, "-f");

  if(!walk_dir(build_dir, _find_src_file, .data = &input)) return_defer(1);
  for(char* cursor=strtok(input.buffer, " ");cursor;cursor = strtok(NULL, " "))
  {
    NOB_VERBOSE nob_log(INFO, "file to remove: %s", cursor);
    cmd_append(&cmd, cursor);
  }

  if (!cmd_run(&cmd,0)) return_defer(2);

defer:
  if(input.buffer){
    free(input.buffer);
    input.buffer = NULL;
  }
  cmd_free(cmd);
  return result;
}

static bool _clean_dir(Walk_Entry entry)
{
  if (strcmp(entry.path, "."))
  {
    nob_delete_file(entry.path);
  }
  return true;
}

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
    cmd_append(&cmd, "-ffreestanding");
    cmd_append(&cmd, "-nostdlib");
    if(fpic) cmd_append(&cmd, "-fPIC");
    cmd_append(&cmd, "-I../"ROOT_SRC_DIR"/src");
    cmd_append(&cmd, "-I../"ROOT_SRC_DIR"/"LIB_DIR"/CResult");

    cmd_append(&cmd, "-D_GNU_SOURCE");

    cmd_append(&cmd, "-DCR_MAX_NUM_OF_CORES=2");

    cmd_append(&cmd, "-DTHREAD_MEM_INFO_SIZE=56");
    cmd_append(&cmd, "-DTHREAD_MEM_INFO_ALIGNEMENT=8");

    cmd_append(&cmd, "-DCR_CONTEXT_SIZE=8");
    cmd_append(&cmd, "-DCR_CONTEXT_ALIGNEMENT=16");

    COMP_VERBOSE cmd_append(&cmd, "-DVERBOSE");
    DEBUG_MODE cmd_append(&cmd, "-g");
    DEBUG_MODE cmd_append(&cmd, "-fsanitize=address,undefined");
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

static int _combine_objs(const char* build_dir, const char* out_dir)
{
  int result=0;
  Cmd cmd = {0};
  Cmd cmd_static = {0};
  Cmd cmd_dyn = {0};
  FindObjFileInput input ={
    .p_cmd = &cmd,
    .buffer = NULL,
    .size = 0,
    .available_space =0,
  };

  set_current_dir(out_dir);

  //cc -Wall -Wextra -r ...
  cmd_append(&cmd, "cc");
  cmd_append(&cmd, "-Wall");
  cmd_append(&cmd, "-Wextra");
  cmd_append(&cmd, "-r");
  cmd_append(&cmd, "-o");
  cmd_append(&cmd, O_FILE".o");

  //ar rcs libfoo.a ...
  cmd_append(&cmd_static, "ar");
  cmd_append(&cmd_static, "rcs");
  cmd_append(&cmd_static, O_FILE".a");

  //cc -Wall -Wextra -shared  ...
  cmd_append(&cmd_dyn, "cc");
  cmd_append(&cmd_dyn, "-Wall");
  cmd_append(&cmd_dyn, "-Wextra");
  cmd_append(&cmd_dyn, "-shared");
  cmd_append(&cmd_dyn, "-o");
  cmd_append(&cmd_dyn, O_FILE".so");


  if(!walk_dir(build_dir, _find_obj_file, .data = &input)) return_defer(1);

  for(char* cursor=strtok(input.buffer, " ");cursor;cursor = strtok(NULL, " "))
  {
    NOB_VERBOSE nob_log(INFO, "file to link: %s", cursor);
    cmd_append(&cmd, cursor);
    cmd_append(&cmd_static, cursor);
    cmd_append(&cmd_dyn, cursor);
  }

  if (!cmd_run(&cmd,0)) return_defer(2);
  if (!cmd_run(&cmd_static,0)) return_defer(2);
  if(fpic){
    if (!cmd_run(&cmd_dyn,0)) return_defer(2);
  }

defer:
  if(input.buffer){
    free(input.buffer);
    input.buffer = NULL;
  }
  cmd_free(cmd);
  if(!set_current_dir(root)) return(2);
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
    if (CHECK_ARG(argv, "-g") || CHECK_ARG(argv, "--debug")) {
      comp_debug_mode= true;
    }

    if (CHECK_ARG(argv, "-h") || CHECK_ARG(argv, "--help")) {
      printf("usage ./nob <options>\n");
      printf("\t\t-vn, --nob_verbose\tnob verbose log\n");
      printf("\t\t-g, --debug\tcompile in debug mode\n");
      printf("\t\t-vc, --comp_verbose\tcompilation verbose log\n");
      printf("\t\t-h, --help\t\tprint this help\n");
      exit(0);
    }
  }

#undef CHECK_ARG
}

int compile_all(void)
{
  int err =0;
  char root_src_dir[PATH_MAX] = {0};
  char lib_dir[PATH_MAX] = {0};
  char hal_dir[PATH_MAX] = {0};

  if(!mkdir_if_not_exists(OUT_DIR)) return 1;
  if(!mkdir_if_not_exists(OUT_DIR"/Include")) return 1;
  if(!mkdir_if_not_exists(BUILD_DIR)) return 1;
  if(!set_current_dir(BUILD_DIR)) return 2;

  NOB_VERBOSE nob_log(INFO, "lib dir: %s", lib_dir);
  NOB_VERBOSE nob_log(INFO, "root_src dir: %s", root_src_dir);
  NOB_VERBOSE nob_log(INFO, "hal dir: %s", hal_dir);

  snprintf(hal_dir, PATH_MAX, "%s/%s", root, HAL_DIR);
  snprintf(root_src_dir, PATH_MAX, "%s/%s/src", root, ROOT_SRC_DIR);
  snprintf(lib_dir, PATH_MAX, "%s/%s/%s", root, ROOT_SRC_DIR, LIB_DIR);

  NOB_VERBOSE nob_log(INFO, "building root_src objs");
  if(!walk_dir(root_src_dir,_compile_files,0)) return err;;

  NOB_VERBOSE nob_log(INFO, "building HAL objs");
  if(!walk_dir(hal_dir,_compile_files,0)) return err;;

  NOB_VERBOSE nob_log(INFO, "building lib objs");
  if(!walk_dir(lib_dir,_compile_files,0)) return err;;

  if(!set_current_dir(root)) return(2);
  return 0;
}

int main(int argc, char** argv)
{
  GO_REBUILD_URSELF(argc, argv);
  int err =0;

  char build_dir[PATH_MAX] = {0};
  char out_dir[PATH_MAX] = {0};

  _parse_input(argc, argv);

  root = get_current_dir_temp();

  snprintf(build_dir, PATH_MAX, "%s/%s", root, BUILD_DIR);
  snprintf(out_dir, PATH_MAX, "%s/%s", root, OUT_DIR);

  NOB_VERBOSE nob_log(INFO, "build dir: %s", build_dir);
  NOB_VERBOSE nob_log(INFO, "out dir: %s", out_dir);

  NOB_VERBOSE nob_log(INFO, "cleaning build dir");
  if(!walk_dir(build_dir,_clean_dir, .post_order = true)) return err;;

  NOB_VERBOSE nob_log(INFO, "cleaning out dir");
  if(!walk_dir(out_dir,_clean_dir, .post_order = true)) return err;;

  fpic = false;

  NOB_VERBOSE nob_log(INFO, "compiling obj NO -fPIC");
  if((err = compile_all())) return err;

  NOB_VERBOSE nob_log(INFO, "combining objs");
  if((err = _combine_objs(build_dir, out_dir))) return(err);

  fpic = true;
  NOB_VERBOSE nob_log(INFO, "cleaning build dir");
  if(!walk_dir(build_dir,_clean_dir, .post_order = true)) return err;;
  nob_delete_file(BUILD_DIR);

  NOB_VERBOSE nob_log(INFO, "compiling obj YES -fPIC");
  if((err = compile_all())) return err;

  NOB_VERBOSE nob_log(INFO, "combining objs");
  if((err = _combine_objs(build_dir, out_dir))) return(err);

  NOB_VERBOSE nob_log(INFO, "copying interfaces");
  if(!copy_directory_recursively(ROOT_SRC_DIR"/src/CRuntime",OUT_DIR"/Include/CRuntime")) return err;
  if(!copy_directory_recursively(ROOT_SRC_DIR"/lib",OUT_DIR"/lib")) return err;
  if((err = _rm_src(OUT_DIR"/Include/CRuntime"))) return err;

  if(!set_current_dir(root)) return(2);

  return 0;
}
