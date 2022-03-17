#include "fake.h"

char* GetCommand(const char* file_name, const Config* config, bool debug)
{
    char* base_name;
    char* result;
    char* flags;

    flags = debug ? config->flags_debug : config->flags_release;
    base_name = FileNameBase(file_name);
    result = StringJoinVariadic(config->compiler, " ", flags, config->include_path,
                                " -c -o ", config->obj_dir, "/", base_name, ".o", NULL);
    free(base_name);

    return result;
}