
#include "advent.hpp"
#include "advent_pico.h"
#include "etl/vector.h"
#include "etl/string.h"

using str_line_t = etl::string<255>;
using map_row_t = etl::vector<char, 256>;
using paper_map_t = advt::xy_map<etl::vector<map_row_t, 256>>;

void read_file(paper_map_t &map)
{
    int count{ 0 };
    for (str_line_t line; advt::getline(line);) {
        printf("%s %d\n", line.c_str(), count);
        count++;
        map.emplace_back();

        for (auto cell : line)
            map.back().push_back(cell);
    }
}

template <typename t_t>
int count_accessible(t_t &map)
{
    const advt::xy_pos dim{ static_cast<int>(map.front().size()), static_cast<int>(map.size()) };
    int count_available{ 0 };

    for (int y = 0; y < dim.y; y++) {
        for (int x = 0; x < dim.x; x++) {
            if (map.at_pos(x, y) != '@')
                continue;

            int count_adjacent{ 0 };

            for (int sub_y = -1; sub_y <= 1; sub_y++) {
                for (int sub_x = -1; sub_x <= 1; sub_x++) {
                    advt::xy_pos check_pos{ x + sub_x, y + sub_y };
                    if (advt::is_pos_on_map(check_pos, dim) && map.at_pos(check_pos) != '.') {
                        count_adjacent++;
                    }
                }
            }

            //the loop self counts, < 4 adjacent rolls
            if (count_adjacent < 5) {
                count_available++;
                //part2 only
                if constexpr (!etl::is_const_v<t_t>) {
                    //mark as x, to be cleaned up
                    map.at_pos(x, y) = 'x';
                }
            }
        }
    }

    return count_available;
}

void remove_accessible(paper_map_t &map)
{
    for (auto &row : map)
        for (auto &cell : row)
            if (cell == 'x')
                cell = '.';
}

int main()
{
    pico_advent_init();

    paper_map_t map;
    read_file(map);

    const auto part1_result = count_accessible(etl::as_const(map));
    printf("%d\n", part1_result);

    int part2_result{ 0 };
    while (const int result = count_accessible(map)) {
        part2_result += result;
        remove_accessible(map);
    }
    printf("%d\n", part2_result);

    pico_advent_finish();
}
