#include <algorithm>
#include <stdio.h>
#include "pico/stdlib.h"

struct safe_part1 {
    int current_position{ 0 };
    int count_zero{ 0 };

    void turn(int rotation)
    {
        current_position += rotation;
        while (current_position < 0)
            current_position += 100;
        while (current_position >= 100)
            current_position -= 100;

        if (current_position == 0)
            count_zero++;
    }
};

struct safe_part2_2 {
    int current_position{ 0 };
    int count_zero{ 0 };

    void turn(int rotation)
    {
        const int goto_position = current_position + rotation;
        int check = rotation > 0 ? 100 : 0;
        int direction = rotation > 0 ? 100 : -100;
        int lower = std::min(current_position, goto_position);
        int higher = std::max(current_position, goto_position);
        if (current_position == 0 && direction == -100)
            check += direction;
        while (lower < check && check < higher) {
            check += direction;
            count_zero++;
        }
        current_position = goto_position;
        while (current_position < 0) {
            current_position += 100;
        }
        while (current_position >= 100) {
            current_position -= 100;
        }
        if (current_position == 0)
            count_zero++;
    }
};

int main()
{
    stdio_init_all();
    int count{ 0 };
    char dir{ 0 };
    int rotation{ 0 };
    safe_part1 safe{ .current_position = 50 };
    safe_part2_2 safe2{ .current_position = 50 };

    while (int ret = scanf(" %c%d", &dir, &rotation) == 2) {
        count++;
        //printf("%c %d %d %d\n", dir, rotation, count, ret);

        if (dir == 'L')
            rotation *= -1;

        safe.turn(rotation);
        safe2.turn(rotation);
    }
    printf("%d\n", safe.count_zero);
    printf("%d\n", safe2.count_zero);

    while (true) {
    }
}
