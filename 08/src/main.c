#include "prod.h"

sem_info sem;

void shutdown_handler(int sig)
{
    printf("Получен сигнал %d, завершение сеанса...\n", sig);

    fflush(stdout);

    delete_sems(&sem);

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    int is_producer;
    srand(time(NULL));
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = shutdown_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    const char* filename;
    if (argc > 3)
    {
        printf("Использование: %s [1 - производитель, 0 - потребитель] [имя файла]", argv[0]);
        return EXIT_FAILURE;
    }
    is_producer = argc > 1 ? atoi(argv[1]) : 1;
    filename = argc > 2 ? argv[2] : FILENAME;

    if (strlen(filename) + strlen(PATHNAME) >= FILENAME_MAX)
    {
        fprintf(stderr, "Слишком длинное имя файла %s!\n", filename);
        return EXIT_FAILURE;
    }
    
    int init = init_sem(&sem, PATHNAME, filename);
    if (init == -1)
        return EXIT_FAILURE;

    if (!is_producer)
    {
        while (1)
        {
            int res = process_file(sem.semid, filename);
            if (res == -1)
            {
                delete_sems(&sem);
                return EXIT_FAILURE;
            }
            sleep(rand() % 3);
        }
    }
    else
    {
        while(1)
        {
            int res = append_file(sem.semid, filename, GENERATOR_MIN, GENERATOR_MAX);
            if (res == -1)
            {
                delete_sems(&sem);
                return EXIT_FAILURE;
            }
            sleep(rand() % 5);
        }
    }
    
    delete_sems(&sem);
    return EXIT_SUCCESS;
}