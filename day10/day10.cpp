#include "advent.hpp"
#include "advent_pico.h"
#include "fraction.hpp"
#include "etl/algorithm.h"
#include "etl/bit.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"

using wiring_t = etl::vector<int, 16>;
using mat_row_t = etl::vector<advt::fraction, 16>;
using mat_t = etl::vector<mat_row_t, 16>;
using indicators_t = etl::vector<char, 16>;
using buttons_t = etl::vector<wiring_t, 16>;
using joltage_t = etl::vector<int, 16>;
using buttons_bits_t = etl::vector<uint32_t, 16>;
using str_line_t = etl::string<255>;
using part1_results_t = etl::vector<uint32_t, 16>;
using token_view_t = etl::vector<etl::string_view, 16>;

struct machine {
    indicators_t indicators;
    buttons_t buttons;
    joltage_t joltage;
};

struct machine_bits {
    uint32_t indicators{ 0 };
    buttons_bits_t buttons;

    machine_bits(const machine &mach)
    {
        for (size_t i = 0; i < mach.indicators.size(); i++) {
            if (mach.indicators.at(i) == '#')
                indicators |= 1U << i;
        }

        for (const auto &button : mach.buttons) {
            uint32_t temp_button{ 0 };
            for (auto wire : button) {
                temp_button |= 1U << wire;
            }
            buttons.push_back(temp_button);
        }
    }
};

machine read_file(const str_line_t & line)
{    
    token_view_t views;
    etl::get_token_list(line, views, "[](){} ", true, views.max_size());

    indicators_t indicators;
    for (auto ind : views[0]) {
        indicators.push_back(ind);
    }

    buttons_t buttons;
    for (auto i = views.begin() + 1; i < views.end() - 1; ++i) {
        token_view_t views2;
        etl::get_token_list(*i, views2, ",", true, views2.max_size());
        wiring_t wiring;
        for (auto view : views2) {
            wiring.push_back(etl::to_arithmetic<int>(view));
        }
        buttons.push_back(wiring);
    }

    joltage_t joltage;
    token_view_t views2;
    etl::get_token_list(views.back(), views2, ",", true, views2.max_size());

    for (auto view : views2) {
        joltage.push_back(etl::to_arithmetic<int>(view));
    }

    return machine{indicators, buttons, joltage};
}

uint32_t part1_try_permutations(const machine_bits &mach)
{
    const auto button_count = static_cast<uint32_t>(mach.buttons.size());
    const uint32_t button_bits = 1U << button_count;
    part1_results_t results;

    for (uint32_t i = 1; i < button_bits; i++) {
        uint32_t result{ 0 };
        for (uint32_t j = 0; j < button_count; j++) {
            const uint32_t bit_position = 1U << j;
            if (i & bit_position) {
                result = result ^ mach.buttons.at(j);
            }
        }
        if (result == mach.indicators) {
            results.push_back(i);
        }
    }

    const auto min = etl::min_element(results.begin(), results.end(), [](auto a, auto b) { return etl::popcount(a) < etl::popcount(b); });

    return etl::popcount(*min);
}

int main()
{
    pico_advent_init();

    int64_t part1_result{0};

    for (str_line_t line; advt::getline(line);) {
        auto machine = read_file(line);
        machine_bits machine_bits(machine);
        
        part1_result += part1_try_permutations(machine_bits);
        
    }

    printf("%lld\n", part1_result);

    pico_advent_finish();
}
