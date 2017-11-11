/*
*
* Specific source codes of gCluster project
*
*/
#include "gCluster.h"

bool isDouble(const char *str)
{
    char* endptr = 0;
    strtod(str, &endptr);

    return !((endptr == str));
}


