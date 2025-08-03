#ifndef CHAT_H
#define CHAT_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define Q_NAME_CLIENT_TO_SERVER "/07_to_server"
#define Q_NAME_SERVER_TO_CLIENT "/07_to_client"

#define FILEMODE S_IRUSR | S_IWUSR

#define SIZE 1024
#define MAX_MSG 10

#define CLOSE_PRIORITY 0

// extern mqd_t mq_talk;
// extern mqd_t mq_listen;

typedef struct run_info
{
    bool is_server;
    char* name;
    mqd_t mq_talk;
    mqd_t mq_listen;
} run_info;

typedef struct msg_t
{
    size_t size;
    unsigned int priority;
    char body[SIZE];
} msg_t;

mqd_t open_connection(const char* name, struct mq_attr* attr, bool is_listener);

int establish_connections(run_info* info);
int close_server(run_info* info);

int talk(const mqd_t listener, const msg_t msg);
msg_t* listen(const mqd_t talker);

#endif