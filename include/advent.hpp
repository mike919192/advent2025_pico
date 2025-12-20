
#include "etl/string.h"
#include <cstdio>

namespace advt
{

template <size_t n>
bool getline(etl::string<n> &line)
{
    line.clear();
    int read_char{ 0 };
    while (read_char = getchar(), read_char != EOF && read_char != '\n' && read_char != '\x1A')
        line.append(1, static_cast<char>(read_char));

    if (read_char == EOF || read_char == '\x1A')
        return false;
    return true;
}

}
