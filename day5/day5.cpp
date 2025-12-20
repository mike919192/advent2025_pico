
#include "advent.hpp"
#include "advent_pico.h"
#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/utility.h"
#include "etl/vector.h"
#include <cstdint>
#include <cstdio>

using str_line_t = etl::string<31>;
using pair_t = etl::pair<int64_t, int64_t>;
using id_ranges_t = etl::vector<pair_t, 256>;

void read_file(id_ranges_t &id_ranges)
{
    int count{ 0 };
    for (str_line_t line; advt::getline(line);) {
        if (line.empty())
            break;
        etl::vector<etl::string_view, 2> views;
        etl::get_token_list(line, views, "-", true, 2);
        int64_t num1 = etl::to_arithmetic<int64_t>(views[0]);
        int64_t num2 = etl::to_arithmetic<int64_t>(views[1]);
        printf("%lld-%lld %d\n", num1, num2, count);
        count++;
        id_ranges.emplace_back(num1, num2);
    }
}

//this algorithm requires the ranges to be sorted
void part2_merge_ranges(id_ranges_t &id_ranges)
{
    for (auto i = id_ranges.begin(); i < id_ranges.end() - 1; i++) {
        for (auto j = i + 1; j < id_ranges.end(); j++) {
            //if (i) second is greater than (j) first than we can merge
            //-1 because we can merge a range that ends with 19 and the next begins with 20
            if ((*i).second >= (*j).first - 1) {
                //(j) second can be less than (i) if the 2nd range is within the first
                (*i).second = etl::max((*i).second, (*j).second);
                id_ranges.erase(j);
                j--;
            } else {
                break;
            }
        }
    }
}

int main()
{
    pico_advent_init();

    id_ranges_t id_ranges;
    read_file(id_ranges);
    etl::sort(id_ranges.begin(), id_ranges.end());
    part2_merge_ranges(id_ranges);
    int64_t part1_result{ 0 };
    int64_t part2_result{ 0 };
    int count{ 0 };

    for (str_line_t line; advt::getline(line);) {
        const int64_t id = etl::to_arithmetic<int64_t>(line);
        printf("%lld %d\n", id, count);
        count++;
        if (etl::any_of(id_ranges.begin(), id_ranges.end(),
                        [&id](const auto &id_range) { return id >= id_range.first && id <= id_range.second; }))
            part1_result++;
    }

    printf("%lld\n", part1_result);

    for (const auto &range : id_ranges)
        part2_result += range.second - range.first + 1;

    printf("%lld\n", part2_result);

    pico_advent_finish();
}
