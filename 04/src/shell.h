#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 32
#define MAX_COMMANDS 32

#define FILEMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWUSR | S_IROTH

typedef enum {
    REDIR_NONE,
    REDIR_INPUT,
    REDIR_OUTPUT,
    REDIR_APPEND,
    REDIR_PIPE
} RedirType;

typedef struct
{
    char* filename;
    RedirType type;
} Redir_t;

typedef struct Command_t
{
    int argc;
    char* argv[MAX_ARGS];
    Redir_t input;
    Redir_t output;
    bool background;
} Command_t;

typedef struct {
    int count;
    Command_t commands[MAX_COMMANDS];
} CommandList;

void init_command(Command_t* cmd);
void free_command_list(CommandList* list);

void print_prompt();
void exec_command_list(const CommandList commands);

bool is_redir(char c);
bool is_ws(char c);
char* skip_ws(char* s);
char* parse_token(char** str);
RedirType parse_redirection(char** str);
int parse_command_line(const char* line, CommandList* result);


#endif