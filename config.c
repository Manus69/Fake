#include "fake.h"

char* GetFieldValue(char* string)
{
    Int     index;

    index = StringFindC(string, COL_SEPARATOR);
    if (index == NOT_FOUND)
        return NULL;

    string += (index + 1);
    while (*string && IsSpace(*string))
        ++ string;

    return string;
}

static Int _iterate(Config* config, const Deck* lines, Uint n_items)
{
    Uint    n;
    char*   line;
    char*   value;
    
    n = 0;
    while (n < n_items)
    {
        line = *(char **)DeckPointAt(lines, n);
        if (!(value = GetFieldValue(line)))
            return WHY_ERROR;
        
        ((char **)config)[n] = value;
        ++ n;
    }

    return WHY_OK;
}

static bool _config_validate(const Deck* lines, Uint n_lines)
{
    Uint    n;
    char*   line;
    
    if (n_lines != N_FIELDS_IN_CONFIG)
        return false;
    
    n = 0;
    while (n < n_lines)
    {
        line = *(char **)DeckPointAt(lines, n);
        if (!StringStartsWith(line, CONFIG_STRINGS[n]))
            return false;

        ++ n;
    }

    return true;
}

static void* _quit(Deck* lines, Deck* all_lines, Config* config)
{
    DeckDestroy(lines);
    DeckDestroy(all_lines);
    free(config);

    return NULL;
}

static bool _line_not_empty(const void* ptr)
{
    return strcmp(*(char **)ptr, "") != 0;
}

Config* ConfigCreate(const char* file_name)
{
    Config* config;
    Deck*   all_lines;
    Deck*   lines;
    Uint    n_items;

    lines = NULL;
    all_lines = NULL;
    config = NULL;

    if (!(all_lines = ReadFileAllLines2(file_name)))
        return NULL;

    if (!(lines = DeckFilter(all_lines, _line_not_empty)))
        return _quit(lines, all_lines, config);

    n_items = DeckNItems(lines);
    SortDeck(lines, CompareCstr);

    if (!_config_validate(lines, n_items))
        return _quit(lines, all_lines, NULL);

    if (!(config = malloc(sizeof(*config))))
        return _quit(lines, all_lines, config);
    
    if (_iterate(config, lines, n_items) == WHY_ERROR)
        return _quit(lines, all_lines, config);

    DeckDestroy(lines);
    DeckDestroy(all_lines);
    WhySavePtr(&config);

    return config;
}