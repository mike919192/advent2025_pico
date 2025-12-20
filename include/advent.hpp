
#include "etl/string.h"
#include <cassert>
#include <cstdio>

namespace advt
{

template <size_t n>
bool getline(etl::string<n> &line, char delim = 0)
{
    line.clear();
    int read_char{ 0 };
    while (read_char = getchar(), read_char != EOF && read_char != '\x1A' && read_char != '\n' && read_char != delim) {
        line.append(1, static_cast<char>(read_char));
    }

    if (read_char == EOF || read_char == '\x1A')
        return false;
    return true;
}

template<typename t_t = int>
t_t char_to_int(char in)
{
    assert(in >= '0' && in <= '9');

    return static_cast<t_t>(in - '0');
}

}
