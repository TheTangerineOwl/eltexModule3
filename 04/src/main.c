#include "shell.h"

int main()
{
    char input[MAX_INPUT_LENGTH];

    CommandList commands;
    printf("Введите 'exit', чтобы выйти.\n");
    while (1)
    {
        print_prompt();

        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
            break;
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "exit") == 0)
            break;

        parse_command_line(input, &commands);

        exec_command_list(commands);
        free_command_list(&commands);
    }
    return 0;
}