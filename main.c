#include "why.h"
#include "fake.h"

bool IsRegularFile(const struct stat* file_stats)
{
    return file_stats->st_mode & S_IFREG;
}

bool IsRegularFileName(const char* file_name)
{
    struct stat stats;

    if (stat(file_name, &stats) < 0)
        return false;
    
    return IsRegularFile(&stats);
}

bool IsRegularFileWRAP(const void* file_name)
{
    return IsRegularFileName(*(const char **)file_name);
}

bool SameMTime(const struct stat* lhs_stats, const struct stat* rhs_stats)
{
    return lhs_stats->st_mtim.tv_sec == rhs_stats->st_mtim.tv_sec;
}

bool SameMTimeName(const char* lhs_name, const char* rhs_name)
{
    struct stat lhs_stats;
    struct stat rhs_stats;

    if (stat(lhs_name, &lhs_stats) < 0)
        return false;
    
    if (stat(rhs_name, &rhs_stats) < 0)
        return false;
    
    return SameMTime(&lhs_stats, &rhs_stats);
}

Deck* GetFilesInDir(DIR* dir)
{
    Deck*           file_names;
    struct dirent*  file;
    char*           name;

    file_names = DeckCreatePtr(NULL, MemDestroy);

    while (true)
    {
        if (!(file = readdir(dir)))
            break ;

        name = strdup(file->d_name);
        DeckPushBack(file_names, &name);
    }

    return file_names;
}

Deck* GetFilesInDirName(const char* dir_name)
{
    DIR*    dir;
    Deck*   files;

    if (!(dir = opendir(dir_name)))
        return NULL;
    
    files = GetFilesInDir(dir);
    closedir(dir);

    return files;
}

void display_file(const struct dirent* file)
{
    struct stat file_stats;

    if (strcmp(file->d_name, ".") == 0)
        return ;

    if (strcmp(file->d_name, "..") == 0)
        return ;

    stat(file->d_name, &file_stats);

    printf("%s %ld\n", file->d_name, file_stats.st_mtim.tv_sec);
}

void test(const char* dir_name)
{
    DIR* dir;
    struct dirent* entry;

    dir = opendir(dir_name);

    while (true)
    {
        entry = readdir(dir);

        if (!entry)
            break ;
        
        display_file(entry);
    }

    closedir(dir);
}

int main()
{
    WhyStart();

    Config* config;

    if (!FileExists(FAKEFILE_NAME))
    {
        if (FakefileCreate() == WHY_ERROR)
            return WhyEnd();
    }

    if (!(config = ConfigCreate(FAKEFILE_NAME)))
        return WhyEnd();

    if (CreateObjDir(config))
        return WhyEnd();

    Deck* src = GetSourceFileNamesToCompile(config);
    PrintDeck(src, PrintCstrP);
    Deck* cmds = GetCompilationCommands(src, config, true);
    DeckDestroy(src);
    PrintDeck(cmds, PrintCstrP);
    DeckDestroy(cmds);

    // struct stat stats;
    // stat("test.c", &stats);
    // printf("%ld %ld\n", stats.st_mtim.tv_sec, stats.st_atim.tv_sec);
    // stat("obj/test.o", &stats);
    // printf("%ld %ld\n", stats.st_mtim.tv_sec, stats.st_atim.tv_sec);

    // ConfigDestroy(config);

    return WhyEnd();
}