#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

void print_prompt() {
    printf("02> ");
    fflush(stdout);
}

void exec_argv(char* argv[])
{
    pid_t pid = fork();
    
    if (pid == 0)  // child
    {
        execvp(argv[0], argv);
        perror("Не удалось выполнить программу!");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)  // parent
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("Не удалось породить процесс!");
    }
}

void parse_argv(char* command, int* argc, char *argv[])
{
    int i = 0;
    char *token = strtok(command, " \t\n");
    while (token != NULL && i < MAX_ARGS - 1) {
        argv[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    *argc = i;
    argv[i] = NULL;
}

int main()
{
    char input[MAX_INPUT_LENGTH];
    int com_argc;
    char *com_argv[MAX_ARGS];
    
    printf("Введите 'exit', чтобы выйти.\n");
    
    while (1)
    {
        print_prompt();
        
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
            break;
        
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "exit") == 0)
            break;
        
        parse_argv(input, &com_argc, com_argv);
        
        if (com_argv[0] != NULL)
            exec_argv(com_argv);
    }
    
    return 0;
}