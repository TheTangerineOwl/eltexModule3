#include "prod.h"

sem_t* init_sem(seminfo* sem, const char* prefix, const char* filename, unsigned int value)
{
    char sem_name[FILENAME_MAX];
    strcat(sem_name, prefix);
    strcat(sem_name, filename);
    sem->sem_name = strdup(sem_name);
    sem->sem = sem_open(sem_name, SEM_FLAGS, SEM_MODE, value);
    return sem->sem;
}

static int del_line_and_close(FILE* file, const char* filename)
{
    char tmp_name[FILENAME_MAX] = {};
    strcat(tmp_name, filename);
    strcat(tmp_name, ".tmp");

    FILE* tmp = fopen(tmp_name, "w");
    if (!tmp)
    {
        perror("process_file (fclose)");
        return -1;
    }
    char line[MAX_GENERATED_LENGTH];
    while (fgets(line, MAX_GENERATED_LENGTH, file))
    {
        fputs(line, tmp);
    }
    fclose(tmp);
    if (fclose(file) == -1)
    {
        perror("process_file (fclose)");
        return -1;
    }

    if (unlink(filename) != 0)
    {
        perror("unlink");
        return -1;
    }
    if (rename(tmp_name, filename) != 0)
    {
        perror("rename");
        return -1;
    }
    return 0;
}

int process_file(const char* filename)
{
    int min, max;
    FILE* file;
    char buffer[MAX_GENERATED_LENGTH];

    if (sem_wait(full) != 0)
    {
        perror("process_file (full P)");
        return -1;
    }
    if (sem_wait(mutex) != 0)
    {
        perror("process_file (mutex P)");
        sem_post(full);
        return -1;
    }
    file = fopen(filename, "r+");
    if (!file)
    {
        perror("process_file (fopen)");
        return -1;
    }
    if (!fgets(buffer, MAX_GENERATED_LENGTH, file))
    {
        perror("process_file (fgets)");
        return -1;
    }

    // копирует все ниже первой (прочитанной выше)
    // строчки и перезаписывает файл
    if (del_line_and_close(file, filename) == -1)
    {
        sem_post(mutex);
        sem_post(empty);
        return -1;
    }

    if (sem_post(mutex) != 0)
    {
        perror("process_file (mutex V)");
        sem_post(empty);
        return -1;
    }
    if (sem_post(empty) != 0)
    {
        perror("process_file (empty V)");
        return -1;
    }

    char* before = strdup(buffer);
    if (find_max_and_min(buffer, &max, &min) == -1)
    {
        perror("process_file (find_max_and_min)");
        return -1;
    }
    printf("Max %d, min %d, str: %s", max, min, before);
    fflush(stdout);

    return 0;
}

int append_file(const char* filename, const int min_num, const int max_num)
{
    FILE* file;

    // srand(time(NULL));
    char* buffer = gen_rand_num_str(min_num, max_num);

    printf("Generated string: %s\n", buffer);

    if (sem_wait(empty) != 0)
    {
        perror("append_file (empty P)");
        return -1;
    }
    if (sem_wait(mutex) != 0)
    {
        perror("append_file (mutex P)");
        sem_post(full);
        return -1;
    }
    file = fopen(filename, "a+");
    if (!file)
    {
        perror("append_file (fopen)");
        return -1;
    }
    if (fprintf(file, "%s\n", buffer) < 0)
    {
        perror("append_file (fprintf)");
        return -1;
    }
    if (fclose(file) == -1)
    {
        perror("append_file (fclose)");
        return -1;
    }
    if (sem_post(mutex) != 0)
    {
        perror("append_file (mutex V)");
        sem_post(full);
        return -1;
    }
    if (sem_post(full) != 0)
    {
        perror("append_file (full V)");
        return -1;
    }

    return 0;
}