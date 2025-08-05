#include "prod.h"

char* gen_rand_num_str(const int min, const int max)
{
    // srand(time(NULL));
    char numstr[NUM_STR_LENGTH];
    char* res = (char*)malloc(sizeof(char) * MAX_GENERATED_LENGTH);
    res[0] = '\0';
    int num, delta = max - min + 1;
    size_t new_size = 1;
    size_t count = (size_t)rand() % GENERATOR_COUNT + 1;
    for (size_t i = 0; i < count; i++)
    {
        num = rand() % delta + min;
        snprintf(numstr, NUM_STR_LENGTH - 1, "%d", num);
        numstr[NUM_STR_LENGTH - 1] = '\0';
        new_size += strlen(numstr);
        if (new_size > MAX_GENERATED_LENGTH)
        {
            fprintf(stderr,
                "Сгенерированная строка не может превышать лимит в %lu символов!\n",
                MAX_GENERATED_LENGTH);
            return res;
        }

        res = strcat(res, numstr);

        if (i < count - 1)
        {
            new_size += 1;
            if (new_size > MAX_GENERATED_LENGTH)
            {
                fprintf(stderr,
                    "Сгенерированная строка не может превышать лимит в %lu символов",
                    MAX_GENERATED_LENGTH);
                return res;
            }
            res = strcat(res, " ");
        }
    }
    return res;
}

int find_max_and_min(char* numstr, int* max, int* min)
{
    char* token = strtok(numstr, " \n");
    int number;
    int cur_max = INT_MIN;
    int cur_min = INT_MAX;
    if (!token)
        return -1;
    while (token != NULL)
    {
        number = atoi(token);
        if (number > cur_max)
            cur_max = number;
        if (number < cur_min)
            cur_min = number;
        token = strtok(NULL, " \n");
    }
    *max = cur_max;
    *min = cur_min;
    return 0;
}

