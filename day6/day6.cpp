
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
        for (size_t i = 0; i < lines_views.size(); ++i) {
            etl::get_token_list(lines_views[i], views, " ", true, 1);
            if (views.size() != i + 1) {
                views.clear();
                break;
            }
            lines_views[i] = etl::string_view(views.back().end(), lines_views[i].end());
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

char part2_decode_operands(const lines_t &lines, size_t &position, operands_t &operands)
{
    char op{ 0 };
    while (position > 0) {
        long scale{ 1 };
        long num{ 0 };
        bool all_spaces{ true };
        if (lines.back().at(position - 1) != ' ')
            op = lines.back().at(position - 1);
        for (auto i = lines.rbegin() + 1; i < lines.rend(); i++) {
            const char num_char = (*i).at(position - 1);
            if (num_char == ' ')
                continue;
            all_spaces = false;
            num += advt::char_to_int(num_char) * scale;
            scale *= 10;
        }
        position--;
        if (!all_spaces)
            operands.push_back(num);
        else
            break;
    }

    return op;
}

int main()
{
    pico_advent_init();

    lines_t lines;

    read_file(lines);

    int64_t part1_result = part1_parse(lines);

    printf("%lld\n", part1_result);

    size_t position{ lines.front().size() };
    int64_t part2_result{ 0 };

    while (position > 0) {
        operands_t operands;
        char op = part2_decode_operands(lines, position, operands);
        part2_result += part1_do_op(op, operands);
        op++;
    }

    printf("%lld\n", part2_result);

    pico_advent_finish();
}
