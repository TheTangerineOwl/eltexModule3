#ifndef CHAT_H
#define CHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define QUEUE_NAME "/app06"
#define PROJ_ID 'A'

#define FILEMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP

#define GET_KEY() ftok(QUEUE_NAME, PROJ_ID)

#define SERVER_ID 10

#define MAX_CLIENTS 32

#define MSG_SIZE sizeof(msg_t)
#define MSG_TEXT_SIZE 1024

typedef struct msgbuf
{
    long mtype;  // используется как id реального получателя
    long sender;
    long receiver;
    char mtext[MSG_TEXT_SIZE];
} msg_t;

#endif