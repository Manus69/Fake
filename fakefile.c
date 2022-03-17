#include "fake.h"

static Int _write_file_strings(Int file)
{
    Uint    n;

    n = 0;
    while (CONFIG_STRINGS[n])
    {
        FileWriteStringInto(file, CONFIG_STRINGS[n]);
        FileWriteStringIntoN(file, CONFIG_VALS[n]);
        ++ n;
    }

    return WHY_OK;
}

Int FakefileCreate()
{
    Int file;

    if ((file = FileCreate(FAKEFILE_NAME)) < 0)
        return WHY_ERROR;

    _write_file_strings(file);
    close(file);

    return WHY_OK;
}