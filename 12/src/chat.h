#ifndef CHAT_H
#define CHAT_H

// #define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h> 
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8080

extern int sockfd;
extern bool is_server;
extern struct sockaddr_in peer_addr;
extern bool peer_known;

void *receive_messages(void *arg);
void init_socket(int port);


#endif