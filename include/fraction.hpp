
#include "etl/gcd.h"
#include "etl/lcm.h"
#include "etl/utility.h"

namespace advt
{

struct fraction {
    int num{ 0 };
    int denom{ 1 };

    fraction() = default;

    fraction(int a) : num(a)
    {
    }

    fraction(int a, int b) : num(a), denom(b)
    {
    }

    void simplify()
    {
        int gcd = etl::gcd(num, denom);
        if (denom < 0)
            gcd *= -1;
        num /= gcd;
        denom /= gcd;
    }

    void reciprocal()
    {
        etl::swap(num, denom);
    }

    // compound assignment (does not need to be a member,
    // but often is, to modify the private members)
    fraction &operator+=(const fraction &rhs)
    {
        if (denom != rhs.denom) {
            const int lcd = etl::lcm(denom, rhs.denom);
            const int scale = lcd / denom;
            const int rhs_scale = lcd / rhs.denom;
            denom = lcd;
            num = (num * scale) + (rhs.num * rhs_scale);
            simplify();
            return *this;
        }
        num += rhs.num;
        simplify();
        return *this; // return the result by reference
    }

    fraction operator-() const
    {
        return { -num, denom };
    }

    fraction &operator-=(const fraction &rhs)
    {
        *this += -rhs;
        return *this; // return the result by reference
    }

    fraction &operator*=(const fraction &rhs)
    {
        num *= rhs.num;
        denom *= rhs.denom;
        simplify();
        return *this; // return the result by reference
    }

    fraction &operator/=(const fraction &rhs)
    {
        num *= rhs.denom;
        denom *= rhs.num;
        simplify();
        return *this; // return the result by reference
    }

    // friends defined inside class body are inline and are hidden from non-ADL lookup
    // passing lhs by value helps optimize chained a+b+c
    // otherwise, both parameters may be const references
    friend fraction operator+(fraction lhs, const fraction &rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator-(fraction lhs, const fraction &rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator*(fraction lhs, const fraction &rhs)
    {
        lhs *= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator/(fraction lhs, const fraction &rhs)
    {
        lhs /= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend bool operator<(const fraction &lhs, const fraction &rhs)
    {
        if (lhs.denom != rhs.denom) {
            const int lcd = etl::lcm(lhs.denom, rhs.denom);
            const int lhs_scale = lcd / lhs.denom;
            const int rhs_scale = lcd / rhs.denom;
            return lhs.num * lhs_scale < rhs.num * rhs_scale;
        }
        return lhs.num < rhs.num; // keep the same order
    }

    friend bool operator>(const fraction &lhs, const fraction &rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const fraction &lhs, const fraction &rhs)
    {
        return lhs.num == rhs.num && lhs.denom == rhs.denom;
    }

    friend bool operator!=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs == rhs);
    }
};

}
