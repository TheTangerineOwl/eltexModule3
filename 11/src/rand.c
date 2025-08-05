#include "prod.h"

int* gen_rand_num_str(int* res_array, int* res_count)
{
    if (!res_array || !res_count)
        return NULL;
    int delta = GENERATOR_MAX - GENERATOR_MIN + 1;
    *res_count = rand() % GENERATOR_COUNT + 1;
    res_array[0] = *res_count;
    for (int i = 1; i <= *res_count; i++)
    {
        res_array[i] = rand() % delta + GENERATOR_MIN;
    }
    return res_array;
}

int find_max_and_min(int* nums, int* max, int* min)
{
    if (!nums || !max || !min)
        return -1;
    int number;
    int cur_max = INT_MIN;
    int cur_min = INT_MAX;
    if (nums[0] < 1)
    {
        fprintf(stderr, "find_max_and_min: count can't be less than 1!\n");
        return -1;
    }
    for (int i = 1; i <= nums[0]; i++)
    {
        number = nums[i];
        if (number > cur_max)
            cur_max = number;
        if (number < cur_min)
            cur_min = number;
    }
    *max = cur_max;
    *min = cur_min;
    return 0;
}

