
#include "advent.hpp"
#include "advent_pico.h"
#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/string_view.h"
#include "etl/string_utilities.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"
#include <cstdint>
#include <cstdio>

using id_string_t = etl::string<15>;
using str_line_t = etl::string<31>;

bool part1_eval_id(int64_t id)
{
    id_string_t id_str;
    etl::to_string(id, id_str);
    size_t str_len = id_str.length();
    if (str_len % 2 != 0)
        return false;
    etl::string_view view1 = etl::string_view(id_str).substr(0, str_len / 2);
    etl::string_view view2 = etl::string_view(id_str).substr(str_len / 2);

    return view1 == view2;
}

bool part2_eval_id(int64_t id)
{
    id_string_t id_str;
    etl::to_string(id, id_str);
    size_t str_len = id_str.length();
    for (size_t i = 2; i <= str_len; ++i) {
        if (str_len % i != 0)
            continue;
        etl::string_view view1 = etl::string_view(id_str).substr(0, str_len / i);
        for (size_t j = str_len / i; j < str_len; j += str_len / i) {
            etl::string_view view2 = etl::string_view(id_str).substr(j, str_len / i);
            if (view1 != view2)
                goto not_invalid;
        }
        return true;
not_invalid:
        continue;
    }

    return false;
}

int main()
{
    pico_advent_init();
    int64_t part1_result{ 0 };
    int64_t part2_result{ 0 };
    int count{ 0 };

    for (str_line_t line; advt::getline(line, ',');) {
        etl::vector<etl::string_view, 2> views;
        etl::get_token_list(line, views, "-", true, 2);
        int64_t num1 = etl::to_arithmetic<int64_t>(views[0]);
        int64_t num2 = etl::to_arithmetic<int64_t>(views[1]);

        printf("%lld %lld %d\n", num1, num2, count);
        count++;

        for (int64_t i = num1; i <= num2; ++i) {
            if (part1_eval_id(i))
                part1_result += i;
            if (part2_eval_id(i))
                part2_result += i;
        }
    }

    printf("%lld\n", part1_result);
    printf("%lld\n", part2_result);

    pico_advent_finish();
}
