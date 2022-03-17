#include "fake.h"

char* GetCommand(const char* file_name, const Config* config, bool debug)
{
    char* base_name;
    char* result;
    char* flags;
    char* include_path;

    flags = debug ? config->flags_debug : config->flags_release;
    base_name = FileNameBase(file_name);
    include_path = (*config->include_path == 0) ? "." : config->include_path;

    result = StringJoinVariadic(config->compiler, " ", flags, " -I ", include_path,
                                " ", file_name, " -c -o ", config->obj_dir, "/",
                                base_name, ".o", NULL);
    free(base_name);
    WhySavePtr(&result);

    return result;
}

Deck* GetCompilationCommands(const Deck* file_names, const Config* config, bool debug)
{
    Deck*   commands;
    void*   ptr;
    char*   command;

    if (!(commands = DeckCreatePtr(NULL, NULL)))
        return NULL;
    
    while ((ptr = DeckNext(file_names)))
    {
        command = GetCommand(*(char **)ptr, config, debug);
        DeckPushBack(commands, &command);
    }

    return commands;
}

Deck* GetAllCommands(const Config* config, bool debug)
{
    Deck* file_names;
    Deck* commands;
    char* target_command;

    if (!(file_names = GetSourceFileNamesToCompile(config)))
        return NULL;
    
    if (!(commands = GetCompilationCommands(file_names, config, debug)))
    {
        DeckDestroy(file_names);
        return NULL;
    }


}

Int RunAllCommands(const Deck* commands)
{
    void*   ptr;
    Int     result;

    while ((ptr = DeckNext(commands)))
    {
        result = system(*(char **)ptr);
        if (result < 0)
            return WHY_ERROR;
    }

    return WHY_OK;
}