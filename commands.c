#include "fake.h"

static char* _get_include_string(const char* string)
{
    Deck*   folder_names;
    Deck*   prefixed_names;
    char*   prefixed_name;
    Uint    n;
    char*   name;

    prefixed_name = NULL;

    if (!(folder_names = StringSplit(string, ' ')))
        return NULL;
    
    if (!(prefixed_names = DeckCreatePtr(NULL, MemDestroy)))
    {
        DeckDestroy(folder_names);
        return NULL;
    }

    n = 0;
    while (n < DeckNItems(folder_names))
    {
        name = *(char **)DeckPointAt(folder_names, n);
        prefixed_name = StringConcat(" -I", name);
        DeckPushBack(prefixed_names, &prefixed_name);
        ++ n;
    }

    prefixed_name = StringjoinDeck(prefixed_names, "");
    DeckDestroy(folder_names);
    DeckDestroy(prefixed_names);
    WhySavePtr(&prefixed_name);

    return prefixed_name;
}

static void _init_variables(const Config* config, bool debug, char** flags, char** include_path)
{
    *flags = debug ? config->flags_debug : config->flags_release;
    *include_path = (*config->include_path == 0) ? " -I." : _get_include_string(config->include_path);

}

char* GetCommand(const char* file_name, const Config* config, bool debug)
{
    char* base_name;
    char* result;
    char* flags;
    char* include_path;

    base_name = FileNameBase(file_name);
    _init_variables(config, debug, &flags, &include_path);

    result = StringJoinVariadic(config->compiler, " ", flags, include_path,
                                " ", file_name, " -c -o ", config->obj_dir, "/",
                                base_name, ".o", NULL);
    free(base_name);
    WhySavePtr(&result);

    return result;
}

char* GetTargetCommand(const Config* config, bool debug)
{
    Deck* obj_files;
    char* file_string;
    char* flags;
    char* include_path;
    char* command;

    if (!(obj_files = GetObjFileNames(config)))
        return NULL;
    
    file_string = StringjoinDeck(obj_files, " ");
    _init_variables(config, debug, &flags, &include_path);

    command = StringJoinVariadic(config->compiler, " ", flags, " ", file_string,
                                    include_path,
                                    " -o ", config->target_name, " ", config->linker_flags, NULL);

    DeckDestroy(obj_files);
    free(file_string);
    WhySavePtr(&command);

    return command;
}

Deck* GetCompilationCommands(const Deck* file_names, const Config* config, bool debug)
{
    Deck*       commands;
    char*       command;
    Iterator    iterator;

    if (!(commands = DeckCreatePtr(NULL, NULL)))
        return NULL;
    
    IteratorInit(&iterator);
    while (DeckNext(file_names, &iterator))
    {
        command = GetCommand(*(char **)iterator.item_pointer, config, debug);
        DeckPushBack(commands, &command);
    }

    return commands;
}

static void* _terminate(const Deck* return_value, Deck* file_names, Deck* commands)
{
    DeckDestroy(file_names);
    DeckDestroy(commands);

    return (void *)return_value;
}

Deck* GetAllCommands(const Config* config, bool debug)
{
    Deck* file_names;
    Deck* commands;
    char* target_command;

    file_names = NULL;
    commands = NULL;
    target_command = NULL;

    if (!(file_names = GetSourceFileNamesToCompile(config)))
        return _terminate(NULL, file_names, commands);
    
    if (!(commands = GetCompilationCommands(file_names, config, debug)))
        return _terminate(NULL, file_names, commands);

    if (!(target_command = GetTargetCommand(config, debug)))
        return _terminate(NULL, file_names, commands);
    
    DeckPushBack(commands, &target_command);

    return _terminate(commands, file_names, NULL);
}

Int RunAllCommands(const Deck* commands)
{
    Int         result;
    Iterator    iterator;

    IteratorInit(&iterator);
    while (DeckNext(commands, &iterator))
    {
        printf("%s\n", *(char **)iterator.item_pointer);
        result = system(*(char **)iterator.item_pointer);
        if (result < 0)
            return WHY_ERROR;
    }

    return WHY_OK;
}