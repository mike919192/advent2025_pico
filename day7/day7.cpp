#include "advent.hpp"
#include "advent_pico.h"
#include "etl/map.h"
#include "etl/string.h"
#include <cstdint>
#include <cstdio>

using str_line_t = etl::string<255>;
using splitter_map_t = etl::map<advt::xy_pos, int64_t, 2048>;
using beam_map_t = etl::map<advt::xy_pos, char, 128>;

advt::xy_pos read_file(splitter_map_t &splitters, beam_map_t &beams)
{
    int y{ 0 };
    int x{ 0 };

    for (str_line_t line; advt::getline(line);) {
        x = 0;

        for (auto line_char : line) {
            if (line_char == '^')
                splitters.insert({ advt::xy_pos{ x, y }, 0 });
            else if (line_char == 'S')
                beams.insert({ advt::xy_pos{ x, y }, 0 });

            x++;
        }

        y++;
    }

    return advt::xy_pos{ x, y };
}

void add_to_map(beam_map_t &active_beams, const advt::xy_pos &pos)
{
    if (!active_beams.contains(pos)) {
        active_beams.insert({ pos, 0 });
    }
}

int64_t part1_split_beams(const splitter_map_t &splitters, beam_map_t &active_beams, const advt::xy_pos &dim)
{
    int64_t split_count{ 0 };
    beam_map_t next_active_beams;
    for (const auto &beam : active_beams) {
        advt::xy_pos next_pos = beam.first + advt::xy_pos{ 0, 1 };
        if (advt::is_pos_on_map(next_pos, dim)) {
            if (splitters.contains(next_pos)) {
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ -1, 0 });
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ 1, 0 });
                split_count++;
            } else {
                next_active_beams.insert({ next_pos, 0 });
            }
        }
    }

    active_beams = next_active_beams;
    return split_count;
}

int64_t part2_trace_beam(const advt::xy_pos &beam, const splitter_map_t &splitters, const advt::xy_pos &dim)
{
    advt::xy_pos local_beam = beam;
    while (advt::is_pos_on_map(local_beam, dim) && !splitters.contains(local_beam))
        local_beam += advt::xy_pos{ 0, 1 };

    if (!advt::is_pos_on_map(local_beam, dim))
        return 1;

    return splitters.at(local_beam);
}

//splitter is ordered map so loop in reverse
//trace beam paths
//if hit bottom add 1
//if hit other splitter add that value
void part2_count_timeline(splitter_map_t &splitters, const advt::xy_pos &dim)
{
    for (auto i = splitters.rbegin(); i != splitters.rend(); i++) {
        int64_t count{ 0 };
        count += part2_trace_beam((*i).first + advt::xy_pos{ -1, 0 }, splitters, dim);
        count += part2_trace_beam((*i).first + advt::xy_pos{ 1, 0 }, splitters, dim);
        (*i).second = count;
    }
}

int main()
{
    pico_advent_init();

    splitter_map_t splitters;
    beam_map_t active_beams;

    const auto dim = read_file(splitters, active_beams);

    int64_t part1_answer{ 0 };

    while (!active_beams.empty())
        part1_answer += part1_split_beams(splitters, active_beams, dim);

    printf("%lld\n", part1_answer);

    part2_count_timeline(splitters, dim);

    printf("%lld\n", (*splitters.begin()).second);

    pico_advent_finish();
}
