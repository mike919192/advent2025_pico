
#include "etl/algorithm.h"
#include "etl/span.h"
#include "etl/vector.h"

namespace advt
{

template <typename t_t, t_t reset_value_t = 0>
struct permutator {
private:
    etl::vector<t_t, 16> m_nums{};
    t_t m_max_value;

public:
    permutator(size_t n, t_t max_value) : m_nums(n, reset_value_t), m_max_value(max_value)
    {
    }

    //return false when the permutation repeats
    bool next_permutation()
    {
        for (auto &num : m_nums) {
            if (num == m_max_value) {
                num = reset_value_t;
            } else {
                num++;
                break;
            }
        }
        return !etl::all_of(m_nums.begin(), m_nums.end(), [](auto i) { return i == reset_value_t; });
    }

    etl::span<const t_t> get_nums()
    {
        return etl::span<const t_t>(m_nums);
    }
};

}