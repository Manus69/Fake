#include "fake.h"

static const Config* _config;

Int CreateObjDir(const Config* config)
{
    if (!FileExists(config->obj_dir))
    {
        return mkdir(config->obj_dir, DIR_PERMISSIONS);       
    }

    return WHY_OK;
}

char* GetName(const char* dir_name, const char* file_name)
{
    if (strcmp(dir_name, ".") == 0)
        return strdup(file_name);
    if (strcmp(dir_name, "./") == 0)
        return strdup(file_name);
    if (StringEndsWith(dir_name, "/"))
        return StringConcat(dir_name, file_name);
    else
        return StringJoin(dir_name, "/", file_name);
}

Deck* GetAllFileNamesInDir(DIR* dir, const char* dir_name)
{
    Deck*           files;
    struct dirent*  file;
    char*           name;

    if (!(files = DeckCreatePtr(NULL, NULL)))
        return NULL;
    
    while (true)
    {
        if (!(file = readdir(dir)))
            break ;
        
        name = GetName(dir_name, file->d_name);
        WhySavePtr(&name);

        DeckPushBack(files, &name);
    }

    return files;
}

Deck* GetAllFileNamesInDirName(const char* name)
{
    DIR*    dir;
    Deck*   files;

    if (!(dir = opendir(name)))
        return NULL;
    
    files = GetAllFileNamesInDir(dir, name);
    closedir(dir);

    return files;
}

Deck* GetAllFileNamesInAllDirs(const Deck* dir_names)
{
    char*   dir_name;
    Uint    n;
    Deck*   all_files;
    Deck*   files;

    all_files = DeckCreatePtr(NULL, NULL);
    n = 0;

    while (n < DeckNItems(dir_names))
    {
        dir_name = *(char **)DeckPointAt(dir_names, n);
        if (!(files = GetAllFileNamesInDirName(dir_name)))
        {
            DeckDestroy(all_files);
            return NULL;
        }

        DeckAppend(all_files, files);
        DeckDestroy(files);
        ++ n;
    }

    return all_files;
}

static void* _copy_wrap(const void* ptr)
{
    return strdup(ptr);
}

Deck* GetAllFileNames(const Config* config)
{
    Deck* dir_names;
    Deck* files;
    Deck* unique;

    if (!(dir_names = StringSplit(config->src_dir, ENTRY_SEPARATOR)))
        return NULL;
    
    if (!(files = GetAllFileNamesInAllDirs(dir_names)))
    {
        DeckDestroy(dir_names);
        return NULL;
    }

    unique = DeckUniqueCopy(files, _copy_wrap, NULL, CompareCstr);
    DeckDestroy(files);
    DeckDestroy(dir_names);

    return unique;
}

static bool _is_source(const char* name, const Config* config)
{
    return StringEndsWith(name, config->file_extension);
}

static bool _is_sourceWRAP(const void* name)
{
    return _is_source(*(char **)name, _config);
}

Deck* GetSourceFileNames(const Config* config)
{
    Deck* file_names;
    Deck* source;

    if (!(file_names = GetAllFileNames(config)))
        return NULL;
    
    _config = config;
    source = DeckFilter(file_names, _is_sourceWRAP);

    DeckDestroy(file_names);

    return source;
}