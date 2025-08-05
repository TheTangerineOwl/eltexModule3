#include "prod.h"

sem_info semap[3];
shm_info shmap[3];

void shutdown_handler(int sig)
{
    printf("Получен сигнал %d, завершение сеанса...\n", sig);
    // cleanup();
    exit(EXIT_SUCCESS);
}

void cleanup(void)
{
    printf("Обработано: %d\n", *shmap[PROCOUNT].shptr);
    fflush(stdout);

    delete_sems(semap);
    delete_all_shm(shmap);
}

int main(void)
{
    srand(time(NULL));
    signal(SIGINT, shutdown_handler);
    atexit(cleanup);

    if (init_sems(semap) == -1)
        exit(EXIT_FAILURE);

    if (init_all_shm(shmap) == -1)
        exit(EXIT_FAILURE);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        while (1)
        {
            int res = process_shm(semap, shmap);
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
            int res = produce_sharray(semap, shmap);
            if (res != 0)
            {
                wait(NULL);

                exit(EXIT_FAILURE);
            }
            sleep(1);
        }
    }
    
    return EXIT_SUCCESS;
}