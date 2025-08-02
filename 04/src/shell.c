#include "shell.h"

void print_prompt()
{
    printf("04> ");
    fflush(stdout);
}

void init_command(Command_t* cmd)
{
    cmd->argc = 0;
    for (int i = 0; i < MAX_ARGS; i++)
        cmd->argv[i] = NULL;
    cmd->input.type = REDIR_NONE;
    cmd->input.filename = NULL;
    cmd->output.type = REDIR_NONE;
    cmd->output.filename = NULL;
    cmd->background = false;
}

void free_command_list(CommandList* list)
{
    Command_t* cmd;
    for (int i = 0; i < list->count; i++)
    {
        cmd = &list->commands[i];
        for (int j = 0; j < cmd->argc; j++)
            free(cmd->argv[j]);
        if (cmd->input.filename)
            free(cmd->input.filename);
        if (cmd->output.filename)
            free(cmd->output.filename);
    }
}

bool is_redir(char c)
{
    return c == '>' || c == '<' || c == '|';
}

bool is_ws(char c)
{
    return c == ' ' || c == '\t';
}

char* skip_ws(char* s)
{
    while (*s && is_ws(*s))
        s++;
    return s;
}

char* parse_token(char** str)
{
    char* s = *str;
    s = skip_ws(s);
    if (!*s)
        return NULL;

    bool in_quotes = false;
    bool in_double_quotes = false;
    char* start = s;
    char* end = s;

    while (*s)
    {
        if (*s == '"')
        {
            in_double_quotes = !in_double_quotes;
            s++;
            continue;
        }
        if (*s == '\'')
        {
            in_quotes = !in_quotes;
            s++;
            continue;
        }

        if (!in_quotes && !in_double_quotes && (is_ws(*s) || is_redir(*s)))
            break;

        end = ++s;
    }

    *str = s;
    size_t len = end - start;
    if (len == 0)
        return NULL;

    char* token = malloc(sizeof(char) * (len + 1));
    strncpy(token, start, len);
    token[len] = '\0';

    if (token[0] == '"' || token[0] == '\'')
    {
        token[len] = '\0';
        memmove(token, token + 1, len);
    }

    return token;
}

RedirType parse_redirection(char** str)
{
    char* s = *str;
    s = skip_ws(s);

    switch (*s)
    {
        case '<':
        {
            *str = s + 1;
            return REDIR_INPUT;
        }
        case '>':
        {
            if (*(s + 1) == '>')
            {
                *str = s + 2;
                return REDIR_APPEND;
            }
            *str = s + 1;
            return REDIR_OUTPUT;
        }
        case '|':
        {
            *str = s + 1;
            return REDIR_PIPE;
        }
        default:
            return REDIR_NONE;
    }
    return REDIR_NONE;
}

int parse_command_line(const char* line, CommandList* result)
{
    result->count = 0;
    if (!line || !*line)
        return 0;

    char* str = strdup(line);
    char* pos = str;
    Command_t* current_cmd = &result->commands[result->count++];
    init_command(current_cmd);

    while (*pos)
    {
        pos = skip_ws(pos);
        if (!*pos)
            break;

        if (*pos == '&')
        {
            current_cmd->background = true;
            pos++;
            continue;
        }

        char* before_redir = pos;
        RedirType redir = parse_redirection(&pos);

        switch (redir)
        {
            case REDIR_PIPE:
            {
                current_cmd->output.type = REDIR_PIPE;
                current_cmd->output.filename = NULL;

                if (result->count >= MAX_COMMANDS)
                {
                    fprintf(stderr, "Превышен максимум команд!\n");
                    free(str);
                    return -1;
                }
                current_cmd = &result->commands[result->count++];
                init_command(current_cmd);
                current_cmd->input.type = REDIR_PIPE;
                continue;
            }
            break;
            case REDIR_INPUT:
            {
                char* filename = parse_token(&pos);
                if (!filename)
                {
                    fprintf(stderr, "Ожидалось имя файла!\n");
                    free(str);
                    return -1;
                }
                if (current_cmd->input.type != REDIR_NONE)
                    {
                        fprintf(stderr, "Поток ввода уже был перенаправлен!\n");
                        free(filename);
                        free(str);
                        return -1;
                    }
                    current_cmd->input.type = redir;
                    current_cmd->input.filename = filename;
                continue;
            }
            break;
            case REDIR_OUTPUT:
            {
                char* filename = parse_token(&pos);
                if (!filename)
                {
                    fprintf(stderr, "Ожидалось имя файла!\n");
                    free(str);
                    return -1;
                }
                if (current_cmd->output.type != REDIR_NONE)
                {
                    fprintf(stderr, "Поток вывода уже был перенаправлен!\n");
                    free(filename);
                    free(str);
                    return -1;
                }
                current_cmd->output.type = redir;
                current_cmd->output.filename = filename;
                continue;
            }
            break;
            default:
            break;
        }

        if (pos != before_redir)
        {
            fprintf(stderr, "Не удалось перенаправить!\n");
            free(str);
            return -1;
        }

        char* arg = parse_token(&pos);
        if (!arg)
            continue;

        if (current_cmd->argc >= MAX_ARGS)
        {
            fprintf(stderr, "Слишком много аргументов!\n");
            free(arg);
            free(str);
            return -1;
        }

        current_cmd->argv[current_cmd->argc++] = arg;
    }

    free(str);
    return 0;
}

void exec_command_list(CommandList commands)
{
    int prev_pipe[2] = {-1, -1};
    int next_pipe[2];
    pid_t pids[MAX_COMMANDS];
    int status;
    Command_t* cmd;
    for (int i = 0; i < commands.count; i++)
    {
        cmd = &commands.commands[i];
        if (i != commands.count - 1)
            if (pipe(next_pipe) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)  // child
        {
            // Перенаправляем ввод
            if (cmd->input.type == REDIR_INPUT)
            {
                int fd = open(cmd->input.filename, O_RDONLY);
                if (fd == -1)
                {
                    perror("Открытие файла для ввода не удалось");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);  // Подставляет fd как stdin
                close(fd);
            }
            else if (cmd->input.type == REDIR_PIPE && prev_pipe[0] != -1)
            {
                dup2(prev_pipe[0], STDIN_FILENO);
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }
            else if (prev_pipe[0] != -1)
            {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            // Перенаправляем вывод
            if (cmd->output.type == REDIR_OUTPUT || cmd->output.type == REDIR_APPEND)
            {
                int flags = O_WRONLY | O_CREAT;
                flags |= cmd->output.type == REDIR_APPEND ? O_APPEND : O_TRUNC;
                int fd = open(cmd->output.filename, flags, FILEMODE);
                if (fd == -1)
                {
                    perror("Открытие файла для вывода не удалось");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            else if (cmd->output.type == REDIR_PIPE && i < commands.count - 1)
            {
                close(next_pipe[0]);
                dup2(next_pipe[1], STDOUT_FILENO);
                close(next_pipe[1]);
            }
            else if (i < commands.count - 1)
            {
                close(next_pipe[0]);
                close(next_pipe[1]);
            }

            execvp(cmd->argv[0], cmd->argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else  // parent
        {
            pids[i] = pid;

            if (prev_pipe[0] != -1)
            {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (i < commands.count - 1)
            {
                prev_pipe[0] = next_pipe[0];
                prev_pipe[1] = next_pipe[1];
            }
        }
    }

    for (int i = 0; i < commands.count; i++)
    {
        if (!commands.commands[i].background)
        {
            waitpid(pids[i], &status, 0);  // ждет завершения всех дочерних процессов
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
                fprintf(stderr, "Команда %d (%s) завершилась с кодом %d\n",
                        i + 1, commands.commands[i].argv[0], WEXITSTATUS(status));
        }
    }
}