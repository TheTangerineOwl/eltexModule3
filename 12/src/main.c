#include "chat.h"

int sockfd;
bool is_server = false;
struct sockaddr_in peer_addr;
bool peer_known = false;

void shutdown_handler(int sig)
{
    printf("Получен сигнал %d, завершение сеанса...\n", sig);
    fflush(stdout);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, shutdown_handler);

    if (argc < 3)
    {
        printf("Usage:\n");
        printf("\tRun as server: %s -s <port>\n", argv[0]);
        printf("\tRun as client: %s -c <peer_ip> <peer_port> [<local_port>]\n", argv[0]);
        return 1;
    }

    pthread_t recv_thread;
    int local_port = DEFAULT_PORT;
    int peer_port;

    if (strcmp(argv[1], "-s") == 0)
    {
        is_server = true;
        local_port = atoi(argv[2]);
        printf("Running as a server on port %d\n", local_port);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: %s -c <peer_ip> <peer_port> [<local_port>]\n", argv[0]);
            return 1;
        }
        
        // Порт подключения
        peer_port = atoi(argv[3]);
        if (argc > 4)
            local_port = atoi(argv[4]);  // свой порт

        memset(&peer_addr, 0, sizeof(peer_addr));
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_port = htons(peer_port);
        // запись IP-адреса сервера, к которому подключаемся
        inet_pton(AF_INET, argv[2], &peer_addr.sin_addr);
        peer_known = true;

        printf("Running as a client, connecting to %s:%d (local port %d)\n",
               argv[2], peer_port, local_port);
    }
    else
    {
        printf("Invalid mode!\n");
        return 1;
    }

    init_socket(local_port);

    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0)
    {
        perror("pthread_create");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (!is_server)
    {
        const char *hello = "Connecting";
        sendto(sockfd, hello, strlen(hello), 0,
            (const struct sockaddr *)&peer_addr, sizeof(peer_addr));
    }

    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (is_server && !peer_known)
            printf("Waiting for peer to connect...\n");
        else
            sendto(sockfd, buffer, strlen(buffer), 0,
                (const struct sockaddr *)&peer_addr, sizeof(peer_addr));
    }

    close(sockfd);
    return 0;
}