#include "advent.hpp"
#include "advent_pico.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/string_utilities.h"
#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"
#include <cassert>
#include <cstddef>
#include <cstdio>

using str_line_t = etl::string<31>;
using shape_t = etl::array<etl::array<char, 3>, 3>;
struct region {
    advt::xy_pos size;
    etl::vector<int, 6> num_shapes;
};

void read_shapes(etl::vector<shape_t, 6> &shapes)
{
    //assume there's 6 shapes 3x3
    for (int i = 0; i < 6; i++) {
        shape_t shape{};
        str_line_t line;
        advt::getline(line);
        for (int j = 0; j < 3; j++) {
            advt::getline(line);
            shape.at(j).at(0) = line.at(0);
            shape.at(j).at(1) = line.at(1);
            shape.at(j).at(2) = line.at(2);
        }
        shapes.emplace_back(shape);
        advt::getline(line);
    }
}

bool read_region(region &reg)
{
    str_line_t line;
    bool more = advt::getline(line);
    if (!more)
        return more;
    etl::vector<etl::string_view, 10> views;
    bool all = etl::get_token_list(line, views, "x: ", true, views.max_size());
    assert(all);
    int x = etl::to_arithmetic<int>(views.at(0));
    int y = etl::to_arithmetic<int>(views.at(1));
    reg.size = advt::xy_pos{ x, y };
    for (auto i = views.begin() + 2; i < views.end(); ++i) {
        reg.num_shapes.push_back(etl::to_arithmetic<int>(*i));
    }
    return true;
}

int main()
{
    pico_advent_init();

    etl::vector<shape_t, 6> shapes;
    read_shapes(shapes);

    //determine perfect packing for lower limit to exclude anything lower
    etl::array<int, 6> perfect_packing{};
    for (size_t i = 0; i < perfect_packing.size(); ++i) {
        for (size_t j = 0; j < shapes.at(i).size(); ++j)
            perfect_packing.at(i) += etl::accumulate(shapes.at(i).at(j).begin(), shapes.at(i).at(j).end(), 0,
                                                     [](auto a, auto b) { return a += b == '#' ? 1 : 0; });
    }

    //determine worst case packing for upper limit to include anything higher
    etl::array<int, 6> worst_packing{ 9, 9, 9, 9, 9, 9 };

    int result{ 0 };

    while (true) {
        region reg;
        const bool more = read_region(reg);
        if (!more)
            break;
        //determine best and worst case size requirements
        etl::array<int, 6> perfect_packing_size{};
        etl::array<int, 6> worst_packing_size{};
        for (size_t i = 0; i < reg.num_shapes.size(); ++i) {
            perfect_packing_size.at(i) = reg.num_shapes.at(i) * perfect_packing.at(i);
            worst_packing_size.at(i) = reg.num_shapes.at(i) * worst_packing.at(i);
        }
        const int best_size = reg.size.x * reg.size.y;
        const int worst_size = ((reg.size.x / 3) * 3) * ((reg.size.y / 3) * 3);
        const int perfect_packing_accum = etl::accumulate(perfect_packing_size.begin(), perfect_packing_size.end(), 0);
        const int worst_packing_accum = etl::accumulate(worst_packing_size.begin(), worst_packing_size.end(), 0);

        if (best_size < perfect_packing_accum) {
            //dont count this
        } else if (worst_packing_accum <= worst_size) {
            result++;
        } else {
            assert(false);
        }
    }

    printf("%d\n", result);

    pico_advent_finish();
}
