
#include "advent.hpp"
#include "advent_pico.h"
#include "etl/string.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"

using str_line_t = etl::string<4095>;
using lines_t = etl::vector<str_line_t, 5>;
using lines_views_t = etl::vector<etl::string_view, 5>;
using operands_t = etl::vector<int64_t, 4>;

void read_file(lines_t &lines)
{
    for (str_line_t line; advt::getline(line);) {
        lines.emplace_back(line);
    }
}

int64_t part1_do_op(char op, const operands_t &operands)
{
    int64_t result{ 0 };
    switch (op) {
    case '*':
        result = 1;
        etl::for_each(operands.begin(), operands.end(), [&result](auto num) { result *= num; });
        break;

    case '+':
        etl::for_each(operands.begin(), operands.end(), [&result](auto num) { result += num; });
        break;

    default:
        assert(true);
        break;
    }

    return result;
}

int64_t part1_parse(const lines_t &lines)
{
    int64_t result{ 0 };
    lines_views_t lines_views;
    for (size_t i = 0; i < lines.size(); ++i) {
        lines_views.emplace_back(lines[i]);
    }
    while (true) {
        lines_views_t views;
        for (auto &line : lines_views) {
            etl::get_token_list(line, views, " ", true, 1);
            if (views.empty())
                break;
            line = etl::string_view(views.back().end(), line.end());
        }

        if (views.empty())
            break;

        operands_t operands;
        for (auto i = views.begin(); i < views.end() - 1; ++i)
            operands.push_back(etl::to_arithmetic<int64_t>(*i));
        char op = views.back().front();
        result += part1_do_op(op, operands);
    }

    return result;
}

int main()
{
    pico_advent_init();

    lines_t lines;

    read_file(lines);

    int64_t part1_result = part1_parse(lines);

    printf("%lld", part1_result);

    pico_advent_finish();
}
