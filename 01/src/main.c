#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

double* to_number(const char *str, double* res)
{
    char *endptr;
    *res = strtod(str, &endptr);
    if (*endptr == '\0' && endptr != str)
        return res;
    return NULL;
}

void print_argv(int start, int end, char* argv[])
{
    double res;
    for (int i = start; i < end; i++)
    {
        if (to_number(argv[i], &res))
            printf("%lf %lf\n", res, res * 2);
        else
            printf("%s\n", argv[i]);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        puts("Список аргументов пуст!");
        return EXIT_SUCCESS;
    }
    
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Не удалось породить процесс!");
        return EXIT_FAILURE;
    }
    else if (pid == 0)  // child
        print_argv(argc / 2, argc, argv);
    else  // parent
    {
        print_argv(1, argc / 2, argv);
        wait(NULL);
    }
    return EXIT_SUCCESS;
}