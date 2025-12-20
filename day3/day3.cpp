
#include "advent.hpp"
#include "advent_pico.h"
#include "etl/span.h"
#include "etl/string.h"
#include "etl/vector.h"
#include <cmath>

using str_line_t = etl::string<127>;
using battery_row_t = etl::vector<uint8_t, 128>;

int64_t find_max_joltage(const battery_row_t &row, int num_batts)
{
    battery_row_t results;
    ptrdiff_t result_distance{ -1 };

    for (int i = 0; i < num_batts; i++) {
        const size_t take = row.size() - (num_batts - (i + 1));
        const size_t drop = result_distance + 1;
        auto row_span = etl::span<const uint8_t>(row).subspan(drop, take - drop);
        //bug in etl max element??
        //if elements are equal it returns last
        //std returns first
        auto result = etl::max_element(row_span.begin(), row_span.end(), [](auto a, auto b) { return a <= b; });
        assert(result != row_span.end());
        result_distance = etl::distance(row.begin(), result);
        results.push_back(*result);
    }

    int64_t out_result{ 0 };
    int64_t scale = static_cast<int64_t>(std::pow(10, results.size() - 1));
    for (auto num : results) {
        out_result += num * scale;
        scale /= 10;
    }
    return out_result;
}

int main()
{
    pico_advent_init();
    int count{ 0 };
    int64_t part1_result{ 0 };
    int64_t part2_result{ 0 };

    for (str_line_t line; advt::getline(line, ',');) {
        printf("%s %d\n", line.c_str(), count);
        count++;
        battery_row_t row;
        for (auto num_char : line)
            row.push_back(advt::char_to_int(num_char));

        part1_result += find_max_joltage(row, 2);
        part2_result += find_max_joltage(row, 12);
    }

    printf("%lld\n", part1_result);
    printf("%lld\n", part2_result);

    pico_advent_finish();
}
