
#include "etl/string.h"
#include "etl/tuple.h"
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

template <typename t_t = int>
t_t char_to_int(char in)
{
    assert(in >= '0' && in <= '9');

    return static_cast<t_t>(in - '0');
}

struct xy_pos {
    int x{ 0 };
    int y{ 0 };

    // compound assignment (does not need to be a member,
    // but often is, to modify the private members)
    xy_pos &operator+=(const xy_pos &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this; // return the result by reference
    }

    xy_pos &operator-=(const xy_pos &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this; // return the result by reference
    }

    // friends defined inside class body are inline and are hidden from non-ADL lookup
    // passing lhs by value helps optimize chained a+b+c
    // otherwise, both parameters may be const references
    friend xy_pos operator+(xy_pos lhs, const xy_pos &rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend xy_pos operator-(xy_pos lhs, const xy_pos &rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    xy_pos operator-() const
    {
        return xy_pos{ -x, -y };
    }

    friend bool operator<(const xy_pos &lhs, const xy_pos &rhs)
    {
        return etl::tie(lhs.y, lhs.x) < etl::tie(rhs.y, rhs.x); // keep the same order
    }

    friend bool operator>(const xy_pos &lhs, const xy_pos &rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const xy_pos &lhs, const xy_pos &rhs)
    {
        return etl::tie(lhs.y, lhs.x) == etl::tie(rhs.y, rhs.x);
    }

    friend bool operator!=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs == rhs);
    }
};

inline bool is_pos_on_map(const xy_pos &pos, const xy_pos &dim)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < dim.x && pos.y < dim.y;
}

template <typename t_t>
struct xy_map : public t_t {
    auto &&at_pos(const xy_pos &pos) const
    {
        return this->at(pos.y).at(pos.x);
    }

    auto &&at_pos(const xy_pos &pos)
    {
        return this->at(pos.y).at(pos.x);
    }

    auto &&at_pos(int x, int y) const
    {
        return this->at(y).at(x);
    }

    auto &&at_pos(int x, int y)
    {
        return this->at(y).at(x);
    }
};

}

template <>
struct etl::hash<advt::xy_pos> {
    size_t operator()(const advt::xy_pos &k) const
    {
        // Compute individual hash values for fields
        // and combine them using XOR
        // and bit shifting:

        return hash<int>{}(k.x) ^ (hash<int>{}(k.y) << 1U);
    }
};
