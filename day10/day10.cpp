#include "advent.hpp"
#include "advent_pico.h"
#include "fraction.hpp"
#include "permutator.hpp"
#include "etl/algorithm.h"
#include "etl/bit.h"
#include "etl/limits.h"
#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/vector.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

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
using free_vars_t = etl::vector<int, 16>;
using vals_found_t = etl::vector<uint8_t, 16>;
using test_vals_t = etl::vector<advt::fraction, 16>;

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

machine read_file(const str_line_t &line)
{
    token_view_t views;
    etl::get_token_list(line, views, "[](){} ", true, views.max_size());

    indicators_t indicators;
    for (auto ind : views.front()) {
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

    return machine{ indicators, buttons, joltage };
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

    const auto min = etl::min_element(results.begin(), results.end(),
                                      [](auto a, auto b) { return etl::popcount(a) < etl::popcount(b); });

    assert(min != results.end());
    return min != results.end() ? etl::popcount(*min) : 0;
}

void print_mat(const mat_t &mat)
{
    if constexpr (false) {
        for (const auto &row : mat) {
            for (const auto &el : row) {
                printf("%d", el.num);
                if (el.denom != 1)
                    printf("/%d", el.denom);
                printf("\t");
            }
            printf("\n");
        }
        printf("------------------------------------------------------------\n");
    }
}

mat_t part2_machine_to_mat(const machine &mach)
{
    mat_t mat(mach.joltage.size());
    {
        size_t i{ 0 };
        for (auto &row : mat) {
            row = mat_row_t(mach.buttons.size() + 1);
            row.back() = mach.joltage.at(i);
            i++;
        }
    }
    {
        size_t i{ 0 };
        for (const auto &button : mach.buttons) {
            for (auto wire : button)
                mat.at(wire).at(i) = 1;
            i++;
        }
    }

    return mat;
}

void part2_row_reduce_mat(mat_t &mat)
{
    size_t col{ 0 };
    for (size_t i = 0; i < mat.size() && col < mat.at(0).size() - 1; i++) {
        bool swapped{ false };
        //swap to make leading 1 match column count
        if (mat.at(i).at(col) == 0) {
            for (size_t j = i; j < mat.size(); j++) {
                if (j == i)
                    continue;
                if (mat.at(j).at(col) != 0) {
                    //vector missing swap?
                    //mat.at(j).swap(mat.at(i));
                    etl::swap(mat.at(j), mat.at(i));
                    swapped = true;
                    break;
                }
            }
            if (!swapped) {
                //increment col and try again
                i--;
                col++;
                continue;
            }
        }

        print_mat(mat);

        //scale the first to 1
        if (mat.at(i).at(col) != 1) {
            const auto scale = mat.at(i).at(col);
            for (size_t k = 0; k < mat.at(0).size(); k++)
                mat.at(i).at(k) /= scale;
        }
        if (mat.at(i).at(col) != 1)
            assert(false);

        print_mat(mat);

        //subtract any above and below it
        for (size_t j = 0; j < mat.size(); j++) {
            if (j == i)
                continue;
            if (mat.at(j).at(col) != 0) {
                const auto scale = mat.at(j).at(col);
                //mat.at(j) -= mat.at(i)
                for (size_t k = 0; k < mat.at(0).size(); k++)
                    mat.at(j).at(k) -= mat.at(i).at(k) * scale;
            }
        }

        print_mat(mat);
        col++;
    }

    //remove any rows with all zeros
    for (auto i = mat.begin(); i < mat.end();) {
        if (etl::all_of((*i).begin(), (*i).end(), [](auto a) { return a == 0; }))
            mat.erase(i);
        else
            ++i;
    }
}

free_vars_t part2_find_free_vars(const mat_t &mat)
{
    free_vars_t free_vars;
    size_t col{ 0 };
    for (size_t i = 0; i < mat.size() && col < mat.at(0).size() - 1; i++) {
        if (mat.at(i).at(col) != 1) {
            //found free_var
            free_vars.emplace_back(col);
            i--;
        }
        col++;
    }

    while (col + 1 < mat.at(0).size()) {
        free_vars.emplace_back(col);
        col++;
    }

    return free_vars;
}

int part2_solve_mat(const mat_t &mat, const free_vars_t &free_vars, etl::span<const int> vals)
{
    auto mat_mut = mat;
    vals_found_t val_found(mat.at(0).size() - 1);
    test_vals_t test_vals(mat.at(0).size() - 1);
    {
        size_t i{ 0 };
        for (auto var : free_vars) {
            val_found.at(var) = true;
            test_vals.at(var) = vals[i];
            i++;
        }
    }

    while (!mat_mut.empty()) {
        //fill in the known vals
        for (auto &row : mat_mut) {
            size_t i{ 0 };
            for (auto var : test_vals) {
                if (val_found.at(i) && row.at(i) != 0) {
                    row.back() -= var * row.at(i);
                    row.at(i) = 0;
                }
                i++;
            }
        }

        //look for new variables that can be assigned values
        for (auto &row : mat_mut) {
            //if there is only one entry we can assign the value
            const int num_nonzeros =
                etl::accumulate(row.begin(), row.end() - 1, 0, [](auto a, auto b) { return a += b.num != 0 ? 1 : 0; });
            if (num_nonzeros == 1) {
                const auto nonzero_iter = etl::find_if(row.begin(), row.end(), [](auto a) { return a != 0; });
                const auto index = etl::distance(row.begin(), nonzero_iter);
                val_found.at(index) = true;
                test_vals.at(index) = row.back();
                row.clear();
            }
        }

        //remove empty arrays
        for (auto i = mat_mut.begin(); i < mat_mut.end();) {
            if ((*i).empty())
                mat_mut.erase(i);
            else
                ++i;
        }
    }

    if (!etl::all_of(val_found.begin(), val_found.end(), [](auto a) { return a == true; }))
        assert(false);

    //if any variables are negative or fractional then values are not a solution
    if (etl::all_of(test_vals.begin(), test_vals.end(), [](auto a) { return a >= 0 && a.denom == 1; })) {
        //if values are a solution, count the button presses, we are looking for the minimum
        return etl::accumulate(test_vals.begin(), test_vals.end(), 0, [](auto a, auto b) { return a += b.num; });
    }

    return -1;
}

int main()
{
    pico_advent_init();

    int64_t part1_result{ 0 };
    int64_t part2_result{ 0 };
    int count{ 0 };

    for (str_line_t line; advt::getline(line);) {
        auto machine = read_file(line);
        machine_bits machine_bits(machine);

        part1_result += part1_try_permutations(machine_bits);

        auto mat = part2_machine_to_mat(machine);
        print_mat(mat);
        part2_row_reduce_mat(mat);

        const auto free_vars = part2_find_free_vars(mat);

        //for now we just set the max search value to the max value in the last column
        //its innefficient but it works
        const auto max_val =
            etl::max_element(mat.begin(), mat.end(), [](const auto &a, const auto &b) { return a.back() < b.back(); });

        advt::permutator<int> perm(free_vars.size(), (*max_val).back().num);
        int min_presses{ etl::numeric_limits<int>::max() };

        do {
            const auto presses = part2_solve_mat(mat, free_vars, perm.get_nums());
            if (presses > 0)
                min_presses = etl::min(min_presses, presses);
        } while (perm.next_permutation());
        if (min_presses == etl::numeric_limits<int>::max())
            assert(false);
        part2_result += min_presses;
        printf("%d\n", count++);
    }

    printf("%lld\n", part1_result);
    printf("%lld\n", part2_result);

    pico_advent_finish();
}
