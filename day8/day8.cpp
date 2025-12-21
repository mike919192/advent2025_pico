#include "advent.hpp"
#include "advent_pico.h"
#include "etl/algorithm.h"
#include "etl/array.h"
#include "etl/map.h"
#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"

using str_line_t = etl::string<31>;
using point_t = etl::array<int, 3>;
using points_t = etl::vector<point_t, 32>;
using points2_t = etl::vector<size_t, 32>;
using points_groups_t = etl::vector<points2_t, 16>;

void read_file(points_t &points)
{
    for (str_line_t line; advt::getline(line);) {
        etl::vector<etl::string_view, 3> views;
        etl::get_token_list(line, views, ",", true, 3);
        auto x = etl::to_arithmetic<int>(views[0]);
        auto y = etl::to_arithmetic<int>(views[1]);
        auto z = etl::to_arithmetic<int>(views[2]);

        points.push_back({ x, y, z });
    }
}

etl::tuple<etl::array<size_t, 2>, int64_t> part1_find_min_distances(const points_t &points, int64_t dist_threshold)
{
    etl::array<size_t, 2> indexes{};
    int64_t min_distance{ etl::numeric_limits<int64_t>::max() };
    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {
            int64_t x1 = (*i)[0];
            int64_t y1 = (*i)[1];
            int64_t z1 = (*i)[2];
            int64_t x2 = (*j)[0];
            int64_t y2 = (*j)[1];
            int64_t z2 = (*j)[2];
            int64_t distance = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));

            //assert(distance != dist_threshold);
            if (distance > dist_threshold && distance < min_distance) {
                min_distance = distance;
                indexes = { static_cast<size_t>(etl::distance(points.begin(), i)),
                            static_cast<size_t>(etl::distance(points.begin(), j)) };
            }
        }
    }

    return { indexes, min_distance };
}

auto part1_find_point_in_group(points_groups_t &groups, size_t point_index)
{
    auto glambda = [&point_index](const auto &a) { return a == point_index; };
    auto glambda2 = [&glambda](const auto &a) { return etl::any_of(a.begin(), a.end(), glambda); };
    return etl::find_if(groups.begin(), groups.end(), glambda2);
}

int64_t part1_find_groups(const points_t &points, points_groups_t &groups, int64_t loops)
{
    etl::array<size_t, 2> indexes{};
    int64_t dist_threshold{ 0 };
    for (int64_t i = 0; i < loops; i++) {
        //find distances in order
        etl::tie(indexes, dist_threshold) = part1_find_min_distances(points, dist_threshold);
        //check if either point is in a group
        auto i1 = part1_find_point_in_group(groups, indexes.at(0));
        auto i2 = part1_find_point_in_group(groups, indexes.at(1));

        //neither one is part of group
        if (i1 == groups.end() && i2 == groups.end()) {
            groups.emplace_back();
            groups.back().emplace_back(indexes.at(0));
            groups.back().emplace_back(indexes.at(1));
        } else if (i1 == i2) {
            //already in the same group nothing happens!
        } else if (i1 != groups.end() && i2 == groups.end()) {
            (*i1).emplace_back(indexes.at(1));
        } else if (i2 != groups.end() && i1 == groups.end()) {
            (*i2).emplace_back(indexes.at(0));
        } else {
            //merge
            (*i1).insert((*i1).end(), (*i2).begin(), (*i2).end());
            groups.erase(i2);
        }

        if (groups.size() == 1 && groups.at(0).size() == points.size()) {
            int64_t x1 = points.at(indexes.at(0)).at(0);
            int64_t x2 = points.at(indexes.at(1)).at(0);
            int64_t mult_x = x1 * x2;
            return mult_x;
        }
    }

    return 0;
}

int main()
{
    pico_advent_init();

    points_t points;

    read_file(points);

    {
        points_groups_t groups;

        part1_find_groups(points, groups, 10);

        etl::sort(groups.begin(), groups.end(), [](const auto &a, const auto &b) { return a.size() > b.size(); });

        const int64_t part1_result = groups.at(0).size() * groups.at(1).size() * groups.at(2).size();

        printf("%lld\n", part1_result);
    }

    points_groups_t groups;

    const int64_t part2_result = part1_find_groups(points, groups, etl::numeric_limits<int64_t>::max());

    printf("%lld\n", part2_result);

    pico_advent_finish();
}
