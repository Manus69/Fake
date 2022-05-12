#ifndef FAKE_H
#define FAKE_H

#include "why.h"

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#define FAKEFILE_NAME ("Fakefile")
#define DIR_PERMISSIONS (0777)
#define N_FIELDS_IN_CONFIG (9)
#define COL_SEPARATOR (':')
#define ENTRY_SEPARATOR (' ')
#define COMPILER_STR ("Compiler:")
#define COMPILER_STR_VAL (" gcc")
#define FILE_EXT_STR ("File extension:")
#define FILE_EXT_VAL (" .c")
#define FLAGS_DBG_STR ("Flags debug:")
#define FLAGS_DBG_VAL (" -Wall -Wextra -fsanitize=address -g")
#define FLAGS_RLS_STR ("Flags release:")
#define FLAGS_RLS_VAL (" -O3")
#define INCLUDE_PATH_STR ("Include path:")
#define INCLUDE_PATH_VAL ("")
#define LINKER_STR ("Linker flags:")
#define LINKER_VAL ("")
#define OBJ_DIR_STR ("Object directory:")
#define OBJ_DIR_VAL (" obj")
#define SRC_DIR_STR ("Source directory:")
#define SRC_DIR_VAL (" .")
#define TARGET_STR ("Target name:")
#define TARGET_VAL ("")

typedef struct Config Config;
extern const char* CONFIG_STRINGS[];
extern const char* CONFIG_VALS[];

struct Config
{
    char* compiler;
    char* file_extension;
    char* flags_debug;
    char* flags_release;
    char* include_path;
    char* linker_flags;
    char* obj_dir;
    char* src_dir;
    char* target_name;
};

Int FakefileCreate();
Int CreateObjDir(const Config* config);
Config* ConfigCreate(const char* file_name);
void ConfigDestroy(Config* config);
Deck* GetAllFileNames(const Config* config);
Deck* GetSourceFileNames(const Config* config);
Deck* GetSourceFileNamesToCompile(const Config* config);
Deck* GetObjFileNames(const Config* config);
Int CreateObjDir(const Config* config);
char* GetCommand(const char* file_name, const Config* config, bool debug);
Deck* GetCompilationCommands(const Deck* file_names, const Config* config, bool debug);
Deck* GetAllCommands(const Config* config, bool debug);
Int RunAllCommands(const Deck* commands);

void DBG_display_time(const struct stat* lhs, const struct stat* rhs);

#endif