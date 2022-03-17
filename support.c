#include "fake.h"

void DBG_display_time(const struct stat* lhs, const struct stat* rhs)
{
    printf("left: % ld\nright: %ld\n", lhs->st_mtim.tv_sec, rhs->st_mtim.tv_sec);
}