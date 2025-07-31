#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64
#define CONV_LENGTH 64

typedef struct Command_t
{
    int argc;
    char** argv;
} Command_t;

void print_prompt() {
    printf("02> ");
    fflush(stdout);
}

int exec_argv(char* argv[])
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
        return status;
    }
    else
    {
        perror("Не удалось породить процесс!");
    }
    return EXIT_SUCCESS;
}

void parse_argv(char* input, Command_t* command)
{
    int i = 0;
    char* savetokenPtr;
    char *token = __strtok_r(input, " \t\n", &savetokenPtr);
    while (token != NULL && i < MAX_ARGS - 1)
    {
        command->argv[i++] = token;
        token = __strtok_r(NULL, " \t\n", &savetokenPtr);
    }
    command->argc = i;
    command->argv[i] = NULL;
}

int parse_input(char* input, Command_t** commands)
{
    int commandsCount = 0;
    char* commandInputs[CONV_LENGTH];
    char* savetokenPtr;
    char* token = __strtok_r(input, "&&", &savetokenPtr);
    while (token != NULL && commandsCount < CONV_LENGTH)
    {
        commandInputs[commandsCount] = token;
        parse_argv(
            commandInputs[commandsCount],
            commands[commandsCount]);
        token = __strtok_r(NULL, "&&", &savetokenPtr);
        commandsCount++;
    }
    
    return commandsCount;
}

int main()
{
    char input[MAX_INPUT_LENGTH];
    int command_count;

    Command_t** commands = (Command_t**)malloc(sizeof(Command_t*) * CONV_LENGTH);
    if (!commands)
        return EXIT_FAILURE;

    for (int i = 0; i < CONV_LENGTH; i++)
    {
        commands[i] = (Command_t*)malloc(sizeof(Command_t));
        commands[i]->argv = (char**)malloc(sizeof(char*) * MAX_ARGS);
        commands[i]->argv[0] = NULL;
    }
    
    printf("Введите 'exit', чтобы выйти.\n");
    
    while (1)
    {
        print_prompt();
        
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
            break;
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "exit") == 0)
            break;
        
        command_count = parse_input(input, commands);
        
        
        for (int i = 0; i < CONV_LENGTH && i < command_count; i++)
        {
            if (exec_argv(commands[i]->argv) != EXIT_SUCCESS)
            {
                printf("Не удалось продолжить выполнение!\n");
                break;
            }
        }
    }
    for (int i = 0; i < CONV_LENGTH; i++)
    {
        free(commands[i]->argv);
        free(commands[i]);
    }
    free(commands);
    return EXIT_SUCCESS;
}