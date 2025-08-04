#include "prod.h"

seminfo sinfo[3];

void shutdown_handler(int sig)
{
    printf("Получен сигнал %d, завершение сеанса...\n", sig);

    fflush(stdout);

    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    sem_unlink(mutex_name);
    free(mutex_name);
    sem_unlink(empty_name);
    free(empty_name);
    sem_unlink(full_name);
    free(full_name);

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = shutdown_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    const char* filename;
    if (argc > 2)
    {
        printf("Использование: %s [имя файла]", argv[0]);
        return EXIT_FAILURE;
    }
    else if (argc < 2)
        filename = FILENAME;
    else
        filename = argv[1];
    if (strlen(filename) + strlen(SEM_NAME_FILE) >= FILENAME_MAX)
    {
        fprintf(stderr, "Слишком длинное имя файла %s!\n", filename);
        return EXIT_FAILURE;
    }
    
    init_sem(&mutex_info, SEM_NAME_FILE, filename, 1);
    if (mutex == SEM_FAILED)
    {
        perror("mutex sem");
        return EXIT_FAILURE;
    }
    init_sem(&empty_info, SEM_NAME_EMPTY, filename, FILE_MAX_LINE_COUNT);
    if (empty == SEM_FAILED)
    {
        perror("empty sem");
        sem_close(mutex);
        return EXIT_FAILURE;
    }
    init_sem(&full_info, SEM_NAME_FULL, filename, 0);
    if (full == SEM_FAILED)
    {
        perror("full sem");
        sem_close(mutex);
        sem_close(empty);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        while (1)
        {
            int res = process_file(filename);
            if (res != 0)
                exit(EXIT_FAILURE);
            sleep(2);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        while(1)
        {
            int res = append_file(filename, GENERATOR_MIN, GENERATOR_MAX);
            if (res != 0)
            {
                wait(NULL);
                sem_close(mutex);
                sem_close(empty);
                sem_close(full);

                sem_unlink(mutex_name);
                free(mutex_name);
                sem_unlink(empty_name);
                free(empty_name);
                sem_unlink(full_name);
                free(full_name);
                return EXIT_FAILURE;
            }
            sleep(1);
        }

        wait(NULL);
    }
    
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    sem_unlink(mutex_name);
    free(mutex_name);
    sem_unlink(empty_name);
    free(empty_name);
    sem_unlink(full_name);
    free(full_name);
    return EXIT_SUCCESS;
}