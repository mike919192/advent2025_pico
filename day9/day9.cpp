#include "advent.hpp"
#include "advent_pico.h"
#include "etl/absolute.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"
#include <cassert>
#include <cstdint>

using str_line_t = etl::string<31>;
using points_t = etl::vector<advt::xy_pos, 512>;

static constexpr advt::xy_pos xy_right{ 1, 0 };
static constexpr advt::xy_pos xy_down{ 0, 1 };
static constexpr advt::xy_pos xy_left{ -1, 0 };
static constexpr advt::xy_pos xy_up{ 0, -1 };

void read_file(points_t &points)
{
    for (str_line_t line; advt::getline(line);) {
        etl::vector<etl::string_view, 2> views;
        etl::get_token_list(line, views, ",", true, 2);
        auto x = etl::to_arithmetic<int>(views[0]);
        auto y = etl::to_arithmetic<int>(views[1]);

        points.push_back(advt::xy_pos{ x, y });
    }
}

int64_t part1_find_areas(const points_t &points)
{
    int64_t max_area{ 0 };

    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {
            const auto i_x = static_cast<int64_t>((*i).x);
            const auto i_y = static_cast<int64_t>((*i).y);
            const auto j_x = static_cast<int64_t>((*j).x);
            const auto j_y = static_cast<int64_t>((*j).y);
            const int64_t area = (etl::absolute(i_x - j_x) + 1) * (etl::absolute(i_y - j_y) + 1);

            if (area > max_area)
                max_area = area;
        }
    }

    return max_area;
}

void normalize_dir(advt::xy_pos &dir)
{
    const int max = etl::max(etl::absolute(dir.x), etl::absolute(dir.y));
    assert(max != 0);
    dir.x /= max;
    dir.y /= max;
}

int determine_turn(const advt::xy_pos &last_dir, const advt::xy_pos &dir)
{
    if ((last_dir == xy_right && dir == xy_down) || (last_dir == xy_down && dir == xy_left) ||
        (last_dir == xy_left && dir == xy_up) || (last_dir == xy_up && dir == xy_right))
        return 90;
    if ((last_dir == xy_down && dir == xy_right) || (last_dir == xy_right && dir == xy_up) ||
        (last_dir == xy_up && dir == xy_left) || (last_dir == xy_left && dir == xy_down))
        return -90;

    assert(false);
    return 0;
}

void gen_keepout_point(const etl::array<advt::xy_pos, 3> &points, points_t &keepout)
{
    auto prev_dir = points[1] - points[0];
    normalize_dir(prev_dir);
    auto next_dir = points[2] - points[1];
    normalize_dir(next_dir);
    int turn = determine_turn(prev_dir, next_dir);
    if (turn > 0) {
        keepout.push_back(points[1] + prev_dir - next_dir);
    } else if (turn < 0) {
        keepout.push_back(points[1] - prev_dir + next_dir);
    } else {
        assert(false);
    }
}

void part2_gen_keepout(const points_t &points, points_t &keepout)
{
    gen_keepout_point({ *(points.end() - 1), *(points.begin()), *(points.begin() + 1) }, keepout);

    for (auto i = points.begin() + 1; i < points.end() - 1; ++i) {
        gen_keepout_point({ *(i - 1), *i, *(i + 1) }, keepout);
    }

    gen_keepout_point({ *(points.end() - 2), *(points.end() - 1), *(points.begin()) }, keepout);
}

bool is_line_intersecting(const etl::array<advt::xy_pos, 2> &line, const etl::array<advt::xy_pos, 2> &line2)
{
    advt::xy_pos dir = line[1] - line[0];
    normalize_dir(dir);

    //handle point
    if (line2[0] == line2[1]) {
        if (dir == xy_right) {
            return line[0].x <= line2[0].x && line[1].x >= line2[0].x && line[0].y == line2[0].y;
        } else if (dir == xy_down) {
            return line[0].y <= line2[0].y && line[1].y >= line2[0].y && line[0].x == line2[0].x;
        }
        assert(false);
    }

    advt::xy_pos dir2 = line2[1] - line2[0];
    normalize_dir(dir2);

    if (dir == xy_right && dir2 == xy_right) { //parallel
        return line[0].y == line2[0].y;
    } else if (dir == xy_down && dir2 == xy_down) {
        return line[0].x == line2[0].x;
    } else if (dir == xy_right && dir2 == xy_down) { //perpindicular
        return line[0].x <= line2[0].x && line[1].x >= line2[0].x && line2[0].y <= line[0].y && line2[1].y >= line[0].y;
    } else if (dir == xy_down && dir2 == xy_right) {
        return line2[0].x <= line[0].x && line2[1].x >= line[0].x && line[0].y <= line2[0].y && line[1].y >= line2[0].y;
    }

    assert(false);
    return false;
}

bool is_line_intersecting_keepout(const points_t &keepout, const etl::array<advt::xy_pos, 2> &line)
{
    for (auto i = keepout.begin(); i < keepout.end() - 1; ++i) {
        auto keepout_line = etl::minmax(*i, *(i + 1));
        if (is_line_intersecting({ keepout_line.first, keepout_line.second }, line))
            return true;
    }
    auto keepout_line = etl::minmax(*(keepout.end() - 1), *keepout.begin());
    return is_line_intersecting({ keepout_line.first, keepout_line.second }, line);
}

bool part2_check_rectangle(const etl::array<advt::xy_pos, 2> &rect, const points_t &keepout)
{
    const auto minmax_x = etl::minmax(rect[0].x, rect[1].x);
    const auto minmax_y = etl::minmax(rect[0].y, rect[1].y);

    if (is_line_intersecting_keepout(keepout, { advt::xy_pos{ minmax_x.first, minmax_y.first },
                                                advt::xy_pos{ minmax_x.second, minmax_y.first } }))
        return false;

    if (is_line_intersecting_keepout(keepout, { advt::xy_pos{ minmax_x.first, minmax_y.first },
                                                advt::xy_pos{ minmax_x.first, minmax_y.second } }))
        return false;

    if (is_line_intersecting_keepout(keepout, { advt::xy_pos{ minmax_x.first, minmax_y.second },
                                                advt::xy_pos{ minmax_x.second, minmax_y.second } }))
        return false;

    if (is_line_intersecting_keepout(keepout, { advt::xy_pos{ minmax_x.second, minmax_y.first },
                                                advt::xy_pos{ minmax_x.second, minmax_y.second } }))
        return false;

    return true;
}

int main()
{
    pico_advent_init();

    points_t points;
    read_file(points);

    //get the max area of all combinations of points
    const auto part1_result = part1_find_areas(points);

    printf("%lld\n", part1_result);

    //keepout is a vector of points outside the original shape
    points_t keepout;
    part2_gen_keepout(points, keepout);

    int64_t part2_result{ 0 };
    //int count{ 0 };

    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {
            //printf("%d\n", count++);
            const auto i_x = static_cast<int64_t>((*i).x);
            const auto i_y = static_cast<int64_t>((*i).y);
            const auto j_x = static_cast<int64_t>((*j).x);
            const auto j_y = static_cast<int64_t>((*j).y);
            const int64_t area = (etl::absolute(i_x - j_x) + 1) * (etl::absolute(i_y - j_y) + 1);

            if (area > part2_result && part2_check_rectangle({ *i, *j }, keepout)) {
                part2_result = area;
            }
        }
    }

    printf("%lld\n", part2_result);

    pico_advent_finish();
}
