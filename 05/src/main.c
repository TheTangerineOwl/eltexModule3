#define _POSIX_C_SOURCE 200809L
// #include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define FILENAME "output.txt"

volatile sig_atomic_t counter = 1;
volatile sig_atomic_t sigint_count = 0;
FILE *file = NULL;

void signal_handler(int sig)
{
    if (file == NULL)
    {
        file = fopen(FILENAME, "a");
        if (file == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }

    fprintf(file, "Получен и обработан сигнал %d\n", sig);
    fflush(file);

    if (sigint_count >= 3)
    {
        fclose(file);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));  // задает нули в sa_flags и sa_mask
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    file = fopen(FILENAME, "w");
    if (file == NULL)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    while (1)
    {
        fprintf(file, "%d\n", counter++);
        fflush(file);
        sleep(1);
    }

    fclose(file);
    return EXIT_SUCCESS;
}