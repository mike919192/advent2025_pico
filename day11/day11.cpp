#include "advent.hpp"
#include "advent_pico.h"
#include "multi_vector.hpp"
#include "etl/string.h"
#include "etl/unordered_map.h"

using str_line_t = etl::string<127>;
using device_label_t = etl::string<3>;
using list_outputs_t = advt::multi_vector<device_label_t, 2048, 636>;
using outputs_label_t = advt::multi_vector_subview<device_label_t, 2048, 636>;
using list_devices_t = etl::unordered_map<device_label_t, outputs_label_t, 636>;
using memo_map_t = etl::unordered_map<device_label_t, int64_t, 636>;

void read_file(list_devices_t &devices, list_outputs_t & list_outputs)
{
    for (str_line_t line; advt::getline(line);) {
        etl::vector<etl::string_view, 24> views;
        bool all = etl::get_token_list(line, views, ": ", true, views.max_size());
        assert(all);
        device_label_t name(views[0]);

        list_outputs.emplace_back();
        outputs_label_t outputs = list_outputs.back();
        for (auto i = views.begin() + 1; i < views.end(); ++i) {
            outputs.push_back(device_label_t(*i));
        }

        devices.insert({ name, outputs });
    }
}

int64_t part1_eval_device(list_devices_t &devices, const device_label_t &dev_name,
                          const device_label_t &dest_name, memo_map_t &memo_map)
{
    auto &outputs = devices.at(dev_name);
    int64_t ret_val{ 0 };
    for (auto &output : outputs) {
        //check if output is already in memo map
        if (memo_map.contains(output)) {
            //printf("Retrieved: %lld at %s\n", memo_map.at(output), output.c_str());
            ret_val += memo_map.at(output);
        } else if (output == dest_name) {
            //printf("Got to dest: %s\n", dest_name.c_str());
            ret_val++;
        } else if (output != "out") {
            //printf("Going into: %s\n", output.c_str());
            ret_val += part1_eval_device(devices, output, dest_name, memo_map);
        }
    }
    //add to memo map
    //printf("Add to memo: %s %lld\n", dev_name.c_str(), ret_val);
    memo_map.insert({ dev_name, ret_val });
    return ret_val;
}

int main()
{
    pico_advent_init();

    list_devices_t devices;
    list_outputs_t list_outputs;
    read_file(devices, list_outputs);

    //pretty standard DP + memoization
    memo_map_t memo_map;
    const auto part1_result = part1_eval_device(devices, "you", "out", memo_map);
    printf("%lld\n", part1_result);

    const etl::array<etl::array<etl::string<3>, 4>, 2> paths{ "svr", "fft", "dac", "out", "svr", "dac", "fft", "out" };

    // The only additional work for part 2 is to do each segment of a possible path ("svr", "fft", "dac", "out") in reverse.
    // After each segment reset the memo table except for the endpoint of the next segment.
    int64_t part2_result{ 0 };
    for (const auto &path : paths) {
        int64_t path_result{ 0 };
        memo_map_t memo_map;
        for (auto i = path.rbegin(); i < path.rend() - 1; ++i) {
            path_result = part1_eval_device(devices, *(i + 1), *i, memo_map);
            memo_map = memo_map_t();
            memo_map.insert({*(i + 1), path_result});
        }
        part2_result += path_result;
    }
    printf("%lld\n", part2_result);

    pico_advent_finish();
}
